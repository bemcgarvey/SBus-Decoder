/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: servo.h                                   //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo output functions             //
/////////////////////////////////////////////////////

#ifndef SERVO_H
#define	SERVO_H

extern volatile uint16_t match;

void initServos(void);
void handleCCP1Int(void);
void handleCCP2Int(void);
void startFrame(void);

#endif	/* SERVO_H */

