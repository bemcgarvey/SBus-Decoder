#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "pwm.h"
#include "sequencer.h"

void initPWMinput(void) {
    T1CONbits.ON = 0;
    T1CONbits.RD16 = 1;
    T1CONbits.CKPS = 0b11; //1:8 pre-scale
    T1GPPS = 0b010011; //RC3
    T1GCONbits.GE = 1;
    T1GCONbits.GPOL = 1;
    T1GCONbits.GSPM = 1;
    T1GCONbits.GTM = 0;
    T1CLK = 0b00001; // Fosc/4
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
    uint16_t pulse = *(uint16_t *)&TMR1L;
    if (pulse < PULSE_LOW) {
        sequencerInput = INPUT_LOW;
    } else if (pulse > PULSE_HIGH) {
        sequencerInput = INPUT_HIGH;
    }
    TMR1H = 0;
    TMR1L = 0;
    PIR3bits.TMR1GIF = 0;
    T1GCONbits.GGO = 1;
}