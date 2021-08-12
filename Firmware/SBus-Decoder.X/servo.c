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
#include "settings.h"

uint16_t calculatePeriod(uint8_t frameRate);

void initServos(void) {
    RA0PPS = 0x0a; //PWM1S1P1_OUT
    RA1PPS = 0x0c; //PWM2S1P1_OUT
    RC0PPS = 0x0e; //PWM3S1P1_OUT
    if (!settings.options.sbusPassthrough) {
        RC1PPS = 0x0f; //PWM3S1P2_OUT
    }
    //Setup NCO for 2048000 MHz
    NCO1CONbits.EN = 0;
    NCO1CONbits.PFM = 0; //FDC mode
    NCO1CLKbits.CKS = 0; //Fosc at 64 MHz
    NCO1ACC = 0;
    NCO1INC = 67109;
    NCO1CONbits.EN = 1;
    //setup PWM
    PWMEN = 0; //all off
    PWM1CLK = 0b1010; //NCO1
    PWM1PR = calculatePeriod(settings.outputs[0].frameRate);
    PWM1S1CFGbits.MODE = 0b000; //left justified;
    PWM1S1P1 = 0; //Start no pulse
    PWM2CLK = 0b1010; //NCO1
    PWM2PR = calculatePeriod(settings.outputs[1].frameRate);
    PWM2S1CFGbits.MODE = 0b000;
    PWM2S1P1 = 0;
    PWM3CLK = 0b1010;
    PWM3PR = calculatePeriod(settings.outputs[2].frameRate);
    PWM3S1CFGbits.MODE = 0b000;
    PWM3S1P1 = 0;
    PWM3S1P2 = 0;
    uint8_t enable = 0;
    if (settings.outputs[0].channel != 0) {
        enable |= 0b001;
    }
    if (settings.outputs[1].channel != 0) {
        enable |= 0b010;
    }
    if (settings.outputs[2].channel != 0 | settings.outputs[3].channel != 0) {
        enable |= 0b100;
    }
    PWMEN = enable; //Turn on active outputs
}

uint16_t calculatePeriod(uint8_t frameRate) {
    switch (frameRate) {
        case FRAME_50HZ: return 40960;
        case FRAME_100HZ: return 20480;
        case FRAME_150HZ: return 13653;
    }
    return 40960; //50HZ default
}