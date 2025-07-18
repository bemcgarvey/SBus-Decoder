/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: sequencer.h                               //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo sequencer functions          //
/////////////////////////////////////////////////////

#ifndef SEQUENCER_H
#define	SEQUENCER_H

enum {INPUT_NONE = 0, INPUT_LOW = 1, INPUT_HIGH = 2};
extern volatile uint8_t sequencerInput;

void initSequencer(void);
void sequencerTasks(void);
void sequencerISR(void);

#endif	/* SEQUENCER_H */

