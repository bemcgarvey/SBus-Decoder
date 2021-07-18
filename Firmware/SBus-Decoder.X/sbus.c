/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: sbus.c                                    //
// Target: PIC16F18313                             // 
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
volatile uint8_t currentByte = 0;
volatile char packetUpdate = 0;
volatile char receivingPacket = 0;
volatile uint16_t throttle = 0;

void initSBus(void) {
    //Setup CLC1
    CLC1CONbits.LC1EN = 0;
    CLC1CONbits.LC1MODE = 0b010; //AND
    CLC1SEL0 = 0;
    CLC1SEL1 = 0;
    CLC1SEL2 = 0;
    CLC1SEL3 = 0;
    CLC1POL = 0b10001110;
    CLC1GLS0 = 0x02;
    CLC1GLS1 = 0;
    CLC1GLS2 = 0;
    CLC1GLS3 = 0;
    CLC1CONbits.LC1EN = 1;
    //Setup USART
    TX1STAbits.BRGH = 1;
    BAUD1CONbits.BRG16 = 1;
    SP1BRGH = 0;
    SP1BRGL = 79; //100000 baud at Fosc = 32 MHz
    TX1STAbits.SYNC = 0;
    //BAUD1CONbits.SCKP = 1; //invert polarity
    RC1STAbits.SPEN = 1;
    RC1STAbits.RX9 = 1; //9 bit rx for parity
    PIE1bits.RCIE = 1;
    RC1STAbits.CREN = 1;
}

void processRxByte(void) {
    uint8_t byte;
    uint8_t parity;
    if (RC1STAbits.OERR == 1) {
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
        currentByte = 0;
    } else if (RC1STAbits.FERR == 1) {
        byte = RC1REG;
        currentByte = 0;
    } else {
        parity = RC1STAbits.RX9D;
        byte = RC1REG;
        if (receivingPacket) {
            if (currentByte == sizeof (rxPacket)) {
                receivingPacket = 0;
                throttle = ((uint16_t)rxPacket.channels[2]  >> 6 | (uint16_t)rxPacket.channels[3]  << 2  |
              (uint16_t)rxPacket.channels[4] << 10) & 0x07ff;
            } else {
                rxPacket.bytes[currentByte] = byte;
                ++currentByte;
            }
        } else {
            if (tickCount >= 2 && byte == SBUS_HEADER) {
                receivingPacket = 1;
                currentByte = 0;
            }
        }
    }
    tickCount = 0;
}