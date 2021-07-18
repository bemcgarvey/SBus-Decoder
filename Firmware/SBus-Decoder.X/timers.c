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

volatile uint8_t tickCount = 0;

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
//    if (tickCount == 250) {
//        tickCount = 0;
//        ledToggle();
//    }
}
