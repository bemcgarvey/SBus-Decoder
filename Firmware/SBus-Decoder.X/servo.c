/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: servo.c                                   //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo output functions             //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include "servo.h"

void initServos(void) {
    //RA0PPS = 0x0a; //PWM1S1P1_OUT
    //RA1PPS = 0x0b; //PWM1S1P2_OUT
    //RC0PPS = 0x0c; //PWM2S1P1_OUT
    //RC1PPS = 0x0d; //PWM2S1P2_OUT
}
