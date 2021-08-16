/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: servo.h                                   //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo output functions             //
/////////////////////////////////////////////////////

#ifndef SERVO_H
#define	SERVO_H

void initSbusServos(void);
void initSequencerServos(void);
void setServo(uint8_t output, uint16_t value);

#endif	/* SERVO_H */

