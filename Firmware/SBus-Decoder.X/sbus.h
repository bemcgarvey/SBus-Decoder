/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: sbus.h                                    //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: SBus functions                     //
/////////////////////////////////////////////////////

#ifndef SBUS_H
#define	SBUS_H

#include <stdint.h>

typedef union {
    struct {
        uint8_t channels[22];
        struct {
            unsigned int : 4;
            unsigned int failsafe : 1;
            unsigned int frameLost : 1;
            unsigned int channel18 : 1;
            unsigned int channel17 : 1;
        };
    };
    uint8_t bytes[23];
} SBusPacket;

extern volatile SBusPacket rxPacket;
extern volatile char packetUpdate;
extern volatile uint16_t throttle;

void initSBus(void);
void processRxByte(void);
uint16_t extractChannel(uint8_t channel);


#endif	/* SBUS_H */

