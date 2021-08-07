/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: led.h                                     //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: LED control macros                 //
/////////////////////////////////////////////////////

#ifndef LED_H
#define	LED_H

#define initLED() {TRISC &= 0b11111011; LATCbits.LATC2 = 0;}
#define ledOn() {LATCbits.LATC2 = 1;}
#define ledOff() {LATCbits.LATC2 = 0;}
#define ledToggle() {LATCbits.LATC2 ^= 1;}

#endif	/* LED_H */

