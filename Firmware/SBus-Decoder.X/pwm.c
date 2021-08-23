/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: pwm.c                                     //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo sequencer functions          //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "pwm.h"
#include "sequencer.h"

volatile int16_t pwmPulse = 0;
volatile uint8_t pulseUpdate = false;

void initPWMinput(void) {
    //Setup CLC2 to route NCO to T1
    CLCSELECT = 1;
    CLCnCONbits.EN = 0;
    CLCnCONbits.MODE = 0b010; //4-input AND
    CLCnPOL = 0b00001110;
    CLCnSEL0 = 0b00011011; //NCO1
    CLCnSEL1 = 0;
    CLCnSEL2 = 0;
    CLCnSEL3 = 0; //CLCIN0PPS
    CLCnGLS0 = 0x02; //Gate input 1 true
    CLCnGLS1 = 0; //no inputs - makes true because of polarity
    CLCnGLS2 = 0;
    CLCnGLS3 = 0;
    CLCnCONbits.EN = 1;
    T1CONbits.ON = 0;
    T1CONbits.RD16 = 1;
    T1CONbits.CKPS = 0b000; //1:1 pre-scale 2048 counts per ms
    T1CONbits.NOT_SYNC = 1; //Don't synch with Fosc/4 //TODO is this best?
    T1GPPS = 0b010011; //RC3
    T1GCONbits.GE = 1;
    T1GCONbits.GPOL = 1;
    T1GCONbits.GSPM = 1;
    T1GCONbits.GTM = 0;
    T1CLK = 0b01110; // CLC2OUT (from NCO)
    T1GATE = 0; //T1GPPS
    IPR3bits.TMR1GIP = 0;
    PIR3bits.TMR1GIF = 0;
    PIE3bits.TMR1GIE = 1;
    TMR1H = 0;
    TMR1L = 0;
    T1CONbits.ON = 1;
    T1GCONbits.GGO = 1;
}

void __interrupt(irq(TMR1G), low_priority, base(8)) T1GISR(void) {
    pwmPulse = (int16_t) (*(uint16_t *) & TMR1L);
    pwmPulse -= 2048;
    if (pwmPulse > 2047) {
        pwmPulse = 2047;
    } else if (pwmPulse < 0) {
        pwmPulse = 0;
    }
    pulseUpdate = true;
    if (pwmPulse < PWM_LOW) {
        sequencerInput = INPUT_LOW;
    } else if (pwmPulse > PWM_HIGH) {
        sequencerInput = INPUT_HIGH;
    }
    TMR1H = 0;
    TMR1L = 0;
    PIR3bits.TMR1GIF = 0;
    T1GCONbits.GGO = 1;
}