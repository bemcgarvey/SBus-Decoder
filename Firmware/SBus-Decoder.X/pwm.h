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

enum {PWM_LOW = 512, PWM_HIGH = 1536};  //Same as SBUS values when using NCO for pulse timer

extern volatile int16_t pwmPulse;
extern volatile uint8_t pulseUpdate;
void initPWMinput(void);

#endif	/* PWM_H */

