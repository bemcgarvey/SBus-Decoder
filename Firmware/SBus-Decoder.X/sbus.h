/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: sbus.h                                    //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: SBus functions                     //
/////////////////////////////////////////////////////

#ifndef SBUS_H
#define	SBUS_H

#include <stdint.h>
#include <stdbool.h>

#define SBUS_NUM_CHANNELS   16
#define SBUS_TIMEOUT        100  //time in ms before failsafe

enum {SBUS_LOW = 512, SBUS_HIGH = 1536};

typedef union {
    struct {
        uint8_t channels[22];
        struct {
			unsigned int channel17 : 1;
            unsigned int channel18 : 1;
            unsigned int frameLost : 1;
            unsigned int failsafe : 1;
            unsigned int : 4;
        };
    };
    uint8_t bytes[23];
} SBusPacket;

extern volatile SBusPacket rxPacket;
extern volatile bool packetUpdate;

void initSBusInput(void);
void initSBusDecoder(void);
int16_t decodeChannel(uint8_t channel);
void sBusTasks(void);

#endif	/* SBUS_H */

