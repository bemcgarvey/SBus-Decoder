/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: timers.h                                  //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Timer functions                    //
/////////////////////////////////////////////////////

#ifndef TIMERS_H
#define	TIMERS_H

#define _XTAL_FREQ   64000000U

extern volatile uint8_t sBusRxTicks;
extern volatile uint8_t sBusPacketTicks;

void initTimer2(void);

#endif	/* TIMERS_H */

