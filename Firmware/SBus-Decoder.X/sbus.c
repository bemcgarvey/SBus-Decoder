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

#include "sbus.h"
#include "timers.h"
#include "led.h"

#define SBUS_HEADER 0x0f
#define SBUS_FOOTER 0x00

volatile bool receivingPacket = false;
volatile bool packetUpdate = false;
volatile uint8_t bytesReceived = 0;
volatile SBusPacket rxPacket;

void initSBus(void) {
    U1CON1bits.ON = 0;
    TRISCbits.TRISC3 = 1;
    U1RXPPS = 0b010011; //RC3
    U1CON0bits.BRGS = 1;
    U1CON0bits.TXEN = 0;
    U1CON0bits.RXEN = 1;
    U1CON2bits.RUNOVF = 1; //synch during overflow
    U1CON2bits.RXPOL = 1; //Inverted
    U1CON2bits.STP = 0b00; //Verify 1 stop bit.  TODO should this be 2 stop bits?
    U1CON0bits.MODE = 0b0011; //8 bit even parity
    U1BRG = 159; //100000 baud
    IPR4bits.U1RXIP = 0;
    PIE4bits.U1RXIE = 1;
    U1CON1bits.ON = 1;
}

void __interrupt(irq(U1RX), low_priority, base(8)) U1RXISR(void) {
    uint8_t rx;
    if (U1ERRIRbits.RXFOIF == 1) {
        //rx overflow error
        rx = U1RXB;
        U1ERRIRbits.RXFOIF = 0;
        receivingPacket = false;
    } else if (U1ERRIRbits.FERIF == 1) {
        //framing error
        rx = U1RXB;
        receivingPacket = false;
    } else if (U1ERRIRbits.PERIF == 1) {
        //parity error
        rx = U1RXB;
        receivingPacket = false;
    } else {
        rx = U1RXB;
        if (receivingPacket) {
            if (bytesReceived == sizeof (rxPacket)) {
                receivingPacket = false;
                packetUpdate = true;
            } else {
                rxPacket.bytes[bytesReceived] = rx;
                ++bytesReceived;
            }
        } else {
            if (sBusRxTicks >= 2 && rx == SBUS_HEADER) {
                receivingPacket = true;
                bytesReceived = 0;
            }
        }
    }
    sBusRxTicks = 0;
}

uint16_t decodeChannel(uint8_t channel) {
    uint16_t pwm = 0;
    switch (channel) {
        //TODO check these against RX12 code for encoding
        case 1: pwm = (uint16_t) rxPacket.channels[0] | ((uint16_t) rxPacket.channels[1] << 8);
            break;
        case 2: pwm = (uint16_t) rxPacket.channels[1] >> 3 | (uint16_t) rxPacket.channels[2] << 5;
            break;
        case 3: pwm = (uint16_t) rxPacket.channels[2] >> 6 | (uint16_t) rxPacket.channels[3] << 2
                    | (uint16_t) rxPacket.channels[4] << 10;
            break;
        case 4: pwm = (uint16_t) rxPacket.channels[4] >> 1 | (uint16_t) rxPacket.channels[5] << 7;
            break;
        case 5: pwm = (uint16_t) rxPacket.channels[5] >> 4 | (uint16_t) rxPacket.channels[6] << 4;
            break;
        default: pwm = 0;
    }
    pwm &= 0x7ff;
    return pwm;
    /*ch_[0]  = static_cast<uint16_t>(buf_[1]       | buf_[2]  << 8 & 0x07FF);
    ch_[1]  = static_cast<uint16_t>(buf_[2]  >> 3 | buf_[3]  << 5 & 0x07FF);
    ch_[2]  = static_cast<uint16_t>(buf_[3]  >> 6 | buf_[4]  << 2  |
              buf_[5] << 10 & 0x07FF);
    ch_[3]  = static_cast<uint16_t>(buf_[5]  >> 1 | buf_[6]  << 7 & 0x07FF);
    ch_[4]  = static_cast<uint16_t>(buf_[6]  >> 4 | buf_[7]  << 4 & 0x07FF);
    ch_[5]  = static_cast<uint16_t>(buf_[7]  >> 7 | buf_[8]  << 1  |
              buf_[9] << 9 & 0x07FF);
    ch_[6]  = static_cast<uint16_t>(buf_[9]  >> 2 | buf_[10] << 6 & 0x07FF);
    ch_[7]  = static_cast<uint16_t>(buf_[10] >> 5 | buf_[11] << 3 & 0x07FF);
    ch_[8]  = static_cast<uint16_t>(buf_[12]      | buf_[13] << 8 & 0x07FF);
    ch_[9]  = static_cast<uint16_t>(buf_[13] >> 3 | buf_[14] << 5 & 0x07FF);
    ch_[10] = static_cast<uint16_t>(buf_[14] >> 6 | buf_[15] << 2  |
              buf_[16] << 10 & 0x07FF);
    ch_[11] = static_cast<uint16_t>(buf_[16] >> 1 | buf_[17] << 7 & 0x07FF);
    ch_[12] = static_cast<uint16_t>(buf_[17] >> 4 | buf_[18] << 4 & 0x07FF);
    ch_[13] = static_cast<uint16_t>(buf_[18] >> 7 | buf_[19] << 1  |
              buf_[20] << 9 & 0x07FF);
    ch_[14] = static_cast<uint16_t>(buf_[20] >> 2 | buf_[21] << 6 & 0x07FF);
    ch_[15] = static_cast<uint16_t>(buf_[21] >> 5 | buf_[22] << 3 & 0x07FF);
     */
}