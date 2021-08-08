/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: serial.h                                  //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: serial functions (non-sBus)        //
/////////////////////////////////////////////////////

#ifndef SERIAL_H
#define	SERIAL_H

#include <stdbool.h>

bool detectSerial(void);
void initSerial(void);

#endif	/* SERIAL_H */

