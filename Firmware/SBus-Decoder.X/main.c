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
#include "servo.h"
#include "settings.h"

void lockPPS(void);
void configInterrupts(void);
void configPMD(void);

void main(void) {
    uint8_t mode = INITIALIZING;
    OSCTUNE = 0;
    //All pins digital outputs
    ANSELA = 0;
    LATA = 0;
    TRISA = 0;
    ANSELC = 0;
    LATC = 0;
    TRISC = 0;
    initLED();
    if (!loadSettings()) {
        if (!setDefaultSettings()) {
            ledOn();
            while(1);
        }
    }
    if (detectSerial()) {
        mode = SERIAL_CONNECTED;
        ledOn();
        initSerial();
        while (1);
    }
    mode = settings.requestedMode;
    configPMD();
    configInterrupts();
    initServos();
    initSBus();
    initTimer2();
    lockPPS();
    int packetCount = 0;
    while (1) {
        if (packetUpdate) {
            packetUpdate = false;
            uint8_t channel;
            channel = settings.outputs[0].channel;
            if (channel != 0) {
                PWM1S1P1 = 2048 + decodeChannel(channel);
            }
            channel = settings.outputs[1].channel;
            if (channel != 0) {
                PWM2S1P1 = 2048 + decodeChannel(channel);
            }
            channel = settings.outputs[2].channel;
            if (channel != 0) {
                PWM3S1P1 = 2048 + decodeChannel(channel);
            }
            channel = settings.outputs[3].channel;
            if (channel != 0) {
                PWM3S1P2 = 2048 + decodeChannel(channel);
            }
            PWMLOAD = 0b111; //Load all
            ++packetCount;
            if (packetCount == 75) {
                ledToggle();
                packetCount = 0;
            }
        }
    }
}

void lockPPS(void) {
    PPSLOCK = 0x55;
    PPSLOCK = 0xaa;
    PPSLOCKbits.PPSLOCKED = 1;
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
    /*
    PMD0 = 0b01000111;
    PMD1 = 0b10000001;
    PMD2 = 0b01100110;
    PMD3 = 0b01110011;
    PMD4 = 0b00100010;
    PMD5 = 0b00000111;
     */
}
