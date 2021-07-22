/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: sbus.c                                    //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: SBus functions                     //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include "sbus.h"
#include "timers.h"
#include "led.h"

#define SBUS_HEADER 0x0f
#define SBUS_FOOTER 0x00

volatile SBusPacket rxPacket;

void initSBus(void) {
    
}

uint16_t extractChannel(uint8_t channel) {
    return 0;
}