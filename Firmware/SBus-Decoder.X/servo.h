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

// NCO settings and pulse values
// Using NCO frequency of 1462128 Hz to give a range of 800us - 2200us for
// sbus values of 0-2047

#define NCO_FREQ        1462128
#define NCOINC_VALUE    0x00bb27 
#define PULSE_BASE      1170        //800us calculated from above NCO frequency
#define PERIOD_50HZ     29243       //Recalculate if NCO frequency changes 
#define PERIOD_100HZ    14621
#define PERIOD_150HZ    9748


void initSbusServos(void);
void initSequencerServos(void);
void initSerialServo(void);
void initReverserServos(void);
void initRandomizerServos(void);
void setServo(uint8_t output, uint16_t value);

#endif	/* SERVO_H */

