/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: timers.h                                  //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Timer functions                    //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include "timers.h"
#include "sbus.h"

volatile uint8_t sBusRxTicks = 0;
volatile uint8_t sBusPacketTicks = SBUS_TIMEOUT;

void initTimer2(void) {
    T2CONbits.ON = 0;
    T2CONbits.CKPS = 0b100;  //1:16
    T2CONbits.OUTPS = 9;     //1:10
    T2TMR = 0;
    T2PR = 100;
    T2HLTbits.PSYNC = 1;
    T2HLTbits.CKSYNC = 1;
    T2HLTbits.MODE = 0; //Free running, software gate
    T2CLKCON = 0b0001; //Fosc/4
    PIR3bits.TMR2IF = 0;
    IPR3bits.TMR2IP = 1;
    PIE3bits.TMR2IE = 1;
    T2CONbits.ON = 1;
}

void __interrupt(irq(TMR2), base(8), high_priority) TMR2ISR(void) {
    ++sBusRxTicks;
    if (sBusPacketTicks != 0) {
        --sBusPacketTicks;
    }
    PIR3bits.TMR2IF = 0;
}