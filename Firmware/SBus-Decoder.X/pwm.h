/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: pwm.h                                     //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo sequencer functions          //
/////////////////////////////////////////////////////

#ifndef PWM_H
#define	PWM_H

enum {PULSE_LOW = 1250 * 4, PULSE_HIGH = 1750 * 4};

extern volatile uint16_t pulse;
extern volatile uint8_t pulseUpdate;
void initPWMinput(void);

#endif	/* PWM_H */

