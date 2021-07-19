/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: servo.c                                   //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo output functions             //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include "servo.h"
#include "timers.h"
#include "sbus.h"

volatile uint16_t match = 9000;

void initServos(void) {
    CCP1CONbits.CCP1EN = 0;
    CCP1CONbits.CCP1MODE = 0b0010; //toggle on match
    CCP1CONbits.CCP1EN = 1;
    PIR4bits.CCP1IF = 0;
    PIE4bits.CCP1IE = 1;
}

void startFrame(void) {
    uint32_t pulse = 8000L * throttle;
    pulse /= 2048;
    match = 9000 + (uint16_t)pulse;
    CCPR1 = 1000;
    startTimer1();
}