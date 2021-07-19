/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: timers.h                                  //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Timer functions                    //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include "timers.h"
#include "led.h"
#include "servo.h"

volatile uint8_t tickCount = 0;
volatile uint8_t frameTickCount = 0;
volatile uint8_t startNewFrame = 0;

void initTimer1(void) {
    T1CONbits.TMR1ON = 0;
    T1CONbits.TMR1CS = 0b00; // Fosc / 4
    T1CONbits.T1CKPS = 0b00; // 1:1 pre-scale
    T1GCONbits.TMR1GE = 0; //Gate disabled
    TMR1H = 0;
    TMR1L = 0;
}

//TODO make this a #define or just put code inline
void startTimer1(void) {
    TMR1H = 0;
    TMR1L = 0;
    T1CONbits.TMR1ON = 1;
}

//TODO make this a #define or just put code inline
void stopTimer1(void) {
    T1CONbits.TMR1ON = 0;
}

void initTimer2(void) {
    T2CONbits.TMR2ON = 0;
    T2CONbits.T2OUTPS = 0b1001; //1:10 post-scale
    T2CONbits.T2CKPS = 0b01; //1:4 pre-scale
    TMR2 = 0;
    PR2 = 200;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
}

void startTimer2(void) {
    T2CONbits.TMR2ON = 1;
}

void handleTimer2Int(void) {
    ++tickCount;
    ++frameTickCount;
    //TODO check frame rate
    if (frameTickCount == 22) {
        frameTickCount = 0;
        startNewFrame = 1;
    }
}

