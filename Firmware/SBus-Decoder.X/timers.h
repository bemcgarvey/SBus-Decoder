/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: timers.h                                  //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Timer functions                    //
/////////////////////////////////////////////////////

#ifndef TIMERS_H
#define	TIMERS_H

extern volatile uint8_t tickCount;

void initTimer1(void);
void startTimer1(void);
void handleTimer1Int(void);
void initTimer2(void);
void startTimer2(void);
void handleTimer2Int(void);

#endif	/* TIMERS_H */

