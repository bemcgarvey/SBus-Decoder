/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: main.c                                    //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: main loop and ISR                  //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "serial.h"
#include "sbus.h"
#include "led.h"
#include "timers.h"
#include "settings.h"
#include "sequencer.h"
#include "reverser.h"
#include "randomizer.h"

uint8_t mode = INITIALIZING;

void lockPPS(void);
void configInterrupts(void);
void configPMD(void);

void main(void) {
    if (PCON0bits.RWDT == 0) {
        mode = WDT;
    }
    PCON0bits.RWDT = 1;
    OSCTUNE = 0;
    //All pins digital outputs except RC3
    //RC3 needs to stay in input so it doesn't detect as serial when in
    //pass-through mode.
    ANSELA = 0;
    LATA = 0;
    TRISA = 0;
    ANSELC = 0;
    LATC = 0;
    TRISC = 0b00001000;
    initLED();
    if (mode != WDT) {
        if (!loadSettings()) {
            if (!setDefaultSettings()) {
                while (1) {
                    ledToggle();
                    __delay_ms(200);
                }
            }
        }
        if (detectSerial()) {
            mode = SERIAL_CONNECTED;
            ledOn();
            initSerial();
            while (1) {
                serialTasks();
            }
        }
    } 
    mode = settings.requestedMode;
    configPMD();
    configInterrupts();
    if (mode == SBUS_DECODER) {
        initSBusDecoder();
    } else if (mode == SERVO_SEQUENCER) {
        initSequencer();
    } else if (mode == SERVO_REVERSER) {
        initReverser();
    } else if (mode == RANDOMIZER) {
        initRandomizer();
    }
    lockPPS();
    WDTCON0bits.PS = 0b00101; //Watchdog timer = 32ms
    WDTCON0bits.SEN = 1;
    while (1) {
        switch (mode) {
            case SBUS_DECODER: 
                sBusTasks();
                break;
            case SERVO_SEQUENCER:
                sequencerTasks();
                break;
            case SERVO_REVERSER:
                reverserTasks();
                break;
            case RANDOMIZER:
                randomizerTasks();
                break;
        }
        CLRWDT();
    }
}

void lockPPS(void) {
    PPSLOCK = 0x55;
    PPSLOCK = 0xaa;
    PPSLOCKbits.PPSLOCKED = 1;
    //Lock priority while were at it
    PRLOCK = 0x55;
    PRLOCK = 0xAA;
    PRLOCKbits.PRLOCKED = 1;
}

void configInterrupts(void) {
    INTCON0bits.IPEN = 1;
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0; // unlock IVT
    IVTBASEU = 0;
    IVTBASEH = 0;
    IVTBASEL = 8;
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 1; // lock IVT
    INTCON0bits.GIEH = 1;
    INTCON0bits.GIEL = 1;
}

void configPMD(void) {
    PMD0 = 0b01111111;
    PMD1 = 0b11100000;
    PMD2 = 0b11101111;
    PMD3 = 0b00111000;
    PMD4 = 0b11111001;
    PMD5 = 0b00000011;
}

void __interrupt(irq(TMR4), high_priority, base(8)) TMR4ISR(void) {
    if (mode == SERVO_SEQUENCER) {
        sequencerISR();
    } else if (mode == RANDOMIZER) {
        randomizerISR();
    }
    PIR10bits.TMR4IF = 0;
}