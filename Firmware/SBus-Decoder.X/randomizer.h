/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: randomizer.h                              //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Random servo output functions      //
/////////////////////////////////////////////////////

#ifndef RANDOMIZER_H
#define	RANDOMIZER_H

void initRandomizer(void);
void randomizerTasks(void);
void randomizerISR(void);

#endif	/* RANDOMIZER_H */

