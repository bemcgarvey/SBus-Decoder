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

#include <stdio.h>

void configPins(void);
void configInterrupts(void);
void configPMD(void);

void main(void) {   
    OSCTUNE = 0;
    initLED();
    if (detectSerial()) {
        ledOn();
        initSerial();
        printf("Serial detected\r\n");
        while (1) {
            while (!PIR8bits.U2RXIF);
            char c = U2RXB;
            if (c == 't') {
                ledToggle();
            }
            if (c == 'd') {
                printf("debug mode\r\n");
            }
        }
    }
    configPins();
    configPMD();
    configInterrupts();
    initServos();
    initSBus();
    while (1) {
        ledToggle();
        __delay_ms(1000);
    }
}

void configPins(void) {
    ANSELA = 0;
    LATA = 0;
    TRISA = 0;
    ANSELC = 0;
    LATC = 0;
    TRISC = 0b00001000;
    PPSLOCK = 0x55;
    PPSLOCK = 0xaa;
    PPSLOCKbits.PPSLOCKED = 0;
    U1RXPPS = 0b010011;  //RC3
    RA0PPS = 0x0a;  //PWM1S1P1_OUT
    RA1PPS = 0x0b;  //PWM1S1P2_OUT
    RC0PPS = 0x0c;  //PWM2S1P1_OUT
    RC1PPS = 0x0d;  //PWM2S1P2_OUT
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
