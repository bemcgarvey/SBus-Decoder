/* 
 * File:   led.h
 * Author: bemcg
 *
 * Created on July 16, 2021, 11:11 PM
 */

#ifndef LED_H
#define	LED_H

#define ledOn() {LATAbits.LATA2 = 1;}
#define ledOff() {LATAbits.LATA2 = 0;}
#define ledToggle() {LATAbits.LATA2 ^= 1;}

#endif	/* LED_H */

