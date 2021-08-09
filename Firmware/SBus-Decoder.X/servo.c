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
    RA0PPS = 0x0a; //PWM1S1P1_OUT
    RA1PPS = 0x0c; //PWM2S1P1_OUT
    RC0PPS = 0x0e; //PWM3S1P1_OUT
    RC1PPS = 0x0f; //PWM3S1P2_OUT
    //RC1PPS = 0x24; //TODO remove after testing
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
    PWM1PR = 40960;  //20ms 50Hz  //TODO enable other values 100Hz, 150Hz??
    PWM1S1CFGbits.MODE = 0b000; //left justified;
    PWM1S1P1 = 0;  //Start no pulse
    
    PWM2CLK = 0b1010; //NCO1
    PWM2PR = 40960;  //20ms 50Hz  //TODO enable other values 100Hz, 150Hz??
    PWM2S1CFGbits.MODE = 0b000; //left justified;
    PWM2S1P1 = 0;  //Start no pulse
    
    PWM3CLK = 0b1010; //NCO1
    PWM3PR = 40960;  //20ms 50Hz  //TODO enable other values 100Hz, 150Hz??
    PWM3S1CFGbits.MODE = 0b000; //left justified;
    PWM3S1P1 = 0;  //Start no pulse
    PWM3S1P2 = 0;
    PWMEN = 0b111;  //All on
}
