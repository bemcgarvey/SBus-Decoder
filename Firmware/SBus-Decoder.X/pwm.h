/* 
 * File:   pwm.h
 * Author: bemcg
 *
 * Created on August 15, 2021, 6:34 PM
 */

#ifndef PWM_H
#define	PWM_H

enum {PULSE_LOW = 1250 * 2, PULSE_HIGH = 1750 * 2};  //TODO adjust these as needed

void initPWMinput(void);

#endif	/* PWM_H */

