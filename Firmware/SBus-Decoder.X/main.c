/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: main.c                                    //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: main loop and ISR                  //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "sbus.h"
#include "led.h"
#include "timers.h"
#include "servo.h"

#define _XTAL_FREQ   32000000U

void configPins(void);
void configInterrupts(void);
void configPMD(void);

void main(void) {
    OSCTUNE = 0;
    configPins();
    configPMD();
    configInterrupts();
    initServos();
    initTimer1();
    initTimer2();
    startTimer2();
    initSBus();
    while (1) {
        if (startNewFrame) {
            startNewFrame = 0;
            startFrame();
        }
        //TODO if tickCount gets too big go to failsafe
        if (throttle >= 1024) {
            ledOn();
        } else {
            ledOff();
        }
    }
}

void configPins(void) {
    ANSELA = 0;
    LATA = 0b00000000;
    TRISA = 0b00101000;
    RXPPS = 0b00101; //RA5
    CLCIN0PPS = 0b00011; //RA3
    RA4PPS = 0b00100; //CLC1OUT
    RA0PPS = 0b01100; //CCP1
    RA1PPS = 0b01101; //CCP2
    PPSLOCK = 0x55;
    PPSLOCK = 0xaa;
    PPSLOCKbits.PPSLOCKED = 1;
}

void configInterrupts(void) {
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
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

void __interrupt() isr(void) {
    while (PIR1bits.RCIF == 1) {
        processRxByte();
    }
    if (PIR1bits.TMR2IF == 1) {
        handleTimer2Int();
        PIR1bits.TMR2IF = 0;
    }
    if (PIR4bits.CCP1IF == 1) {
        if (CCP1CONbits.CCP1OUT == 1) {
            CCPR1H = (uint8_t)(match >> 8);
            CCPR1L = (uint8_t) match;
        } else {
            stopTimer1();
        }
        PIR4bits.CCP1IF = 0;
    }
}