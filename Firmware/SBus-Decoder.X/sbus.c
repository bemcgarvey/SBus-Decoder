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
#include "settings.h"
#include "servo.h"

#define SBUS_HEADER 0x0f
#define SBUS_FOOTER 0x00

volatile bool receivingPacket = false;
volatile bool packetUpdate = false;
volatile uint8_t bytesReceived = 0;
volatile SBusPacket rxPacket;
bool failsafeEngaged = 0;

void initSBusInput(void) {
    U1CON1bits.ON = 0;
    TRISCbits.TRISC3 = 1;
    U1RXPPS = 0b010011; //RC3
    U1CON0bits.BRGS = 1;
    U1CON0bits.TXEN = 0;
    U1CON0bits.RXEN = 1;
    U1CON2bits.RUNOVF = 1; //synch during overflow
    U1CON2bits.RXPOL = 1; //Inverted
    U1CON2bits.STP = 0b00; //Verify 1 stop bit.
    U1CON0bits.MODE = 0b0011; //8 bit even parity
    U1BRG = 159; //100000 baud
    IPR4bits.U1RXIP = 0;
    PIE4bits.U1RXIE = 1;
    if (settings.options.sbusPassthrough3 || settings.options.sbusPassthrough4) {
        CLCIN0PPS = 0b010011; //RC3
        if (settings.options.sbusPassthrough4) {
            RC1PPS = 0x01; //CLC1OUT
        }
        if (settings.options.sbusPassthrough3) {
            RC0PPS = 0x01; //CLC1OUT
        }
        CLCSELECT = 0;
        CLCnCONbits.EN = 0;
        CLCnCONbits.MODE = 0b010; //4-input AND
        CLCnPOL = 0b00001110;
        CLCnSEL0 = 0;
        CLCnSEL1 = 0;
        CLCnSEL2 = 0;
        CLCnSEL3 = 0; //CLCIN0PPS
        CLCnGLS0 = 0x02; //Gate input 1 true
        CLCnGLS1 = 0; //no inputs - makes true because of polarity
        CLCnGLS2 = 0;
        CLCnGLS3 = 0;
        CLCnCONbits.EN = 1;
    }
    U1CON1bits.ON = 1;
    initTimer2();
}

void initSBusDecoder(void) {
    initSbusServos();
    initSBusInput();
}

void sBusTasks(void) {
    static int packetCount = 0;
    int16_t pulse;
    if (!failsafeEngaged && sBusPacketTicks == 0) {
        failsafeEngaged = true;
        ledOff();
        if (settings.outputs[0].failsafeMode == FAIL_OFF) {
            PWM1S1P1 = 0;
        } else if (settings.outputs[0].failsafeMode == FAIL_NEUTRAL) {
            pulse = 1023 + settings.outputs[0].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM1S1P1 = (uint16_t) (2048 + pulse);
        }
        if (settings.outputs[1].failsafeMode == FAIL_OFF) {
            PWM2S1P1 = 0;
        } else if (settings.outputs[1].failsafeMode == FAIL_NEUTRAL) {
            pulse = 1023 + settings.outputs[1].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM2S1P1 = (uint16_t) (2048 + pulse);
        }
        if (settings.outputs[2].failsafeMode == FAIL_OFF) {
            PWM3S1P1 = 0;
        } else if (settings.outputs[2].failsafeMode == FAIL_NEUTRAL) {
            pulse = 1023 + settings.outputs[2].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM3S1P1 = (uint16_t) (2048 + pulse);
        }
        if (settings.outputs[3].failsafeMode == FAIL_OFF) {
            PWM3S1P2 = 0;
        } else if (settings.outputs[3].failsafeMode == FAIL_NEUTRAL) {
            pulse = 1023 + settings.outputs[3].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM3S1P2 = (uint16_t) (2048 + pulse);
        }
        PWMLOAD = 0b111; //Load all
    }
    if (packetUpdate) {
        packetUpdate = false;
        failsafeEngaged = false;
        uint8_t channel;
        channel = settings.outputs[0].channel;
        if (channel != 0) {
            if (settings.outputs[0].reverse) {
                pulse = 2047 - decodeChannel(channel);
            } else {
                pulse = decodeChannel(channel);
            }
            pulse += settings.outputs[0].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM1S1P1 = (uint16_t) (2048 + pulse);
        }
        channel = settings.outputs[1].channel;
        if (channel != 0) {
            if (settings.outputs[1].reverse) {
                pulse = 2047 - decodeChannel(channel);
            } else {
                pulse = decodeChannel(channel);
            }
            pulse += settings.outputs[1].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM2S1P1 = (uint16_t) (2048 + pulse);
        }
        channel = settings.outputs[2].channel;
        if (channel != 0) {
            if (settings.outputs[2].reverse) {
                pulse = 2047 - decodeChannel(channel);
            } else {
                pulse = decodeChannel(channel);
            }
            pulse += settings.outputs[2].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM3S1P1 = (uint16_t) (2048 + pulse);
        }
        channel = settings.outputs[3].channel;
        if (channel != 0) {
            if (settings.outputs[3].reverse) {
                pulse = 2047 - decodeChannel(channel);
            } else {
                pulse = decodeChannel(channel);
            }
            pulse += settings.outputs[3].subTrim;
            if (pulse < 0) {
                pulse = 0;
            } else if (pulse > 2047) {
                pulse = 2047;
            }
            PWM3S1P2 = (uint16_t) (2048 + pulse);
        }
        PWMLOAD = 0b111; //Load all
        ++packetCount;
        if (packetCount == 300) {
            ledOn();
        } else if (packetCount == 305) {
            ledOff();
            packetCount = 0;
        }
    }
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
                sBusPacketTicks = SBUS_TIMEOUT;
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

int16_t decodeChannel(uint8_t channel) {
    uint16_t pwm = 0;
    switch (channel) {
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
        case 6: pwm = (uint16_t) rxPacket.channels[6] >> 7 | (uint16_t) rxPacket.channels[7] << 1
                    | (uint16_t) rxPacket.channels[8] << 9;
            break;
        case 7: pwm = (uint16_t) rxPacket.channels[8] >> 2 | (uint16_t) rxPacket.channels[9] << 6;
            break;
        case 8: pwm = (uint16_t) rxPacket.channels[9] >> 5 | (uint16_t) rxPacket.channels[10] << 3;
            break;
        case 9: pwm = (uint16_t) rxPacket.channels[11] | (uint16_t) rxPacket.channels[12] << 8;
            break;
        case 10: pwm = (uint16_t) rxPacket.channels[12] >> 3 | (uint16_t) rxPacket.channels[13] << 5;
            break;
        case 11: pwm = (uint16_t) rxPacket.channels[13] >> 6 | (uint16_t) rxPacket.channels[14] << 2
                    | (uint16_t) rxPacket.channels[15] << 10;
            break;
        case 12: pwm = (uint16_t) rxPacket.channels[15] >> 1 | (uint16_t) rxPacket.channels[16] << 7;
            break;
        case 13: pwm = (uint16_t) rxPacket.channels[16] >> 4 | (uint16_t) rxPacket.channels[17] << 4;
            break;
        case 14: pwm = (uint16_t) rxPacket.channels[17] >> 7 | (uint16_t) rxPacket.channels[18] << 1
                    | (uint16_t) rxPacket.channels[19] << 9;
            break;
        case 15: pwm = (uint16_t) rxPacket.channels[19] >> 2 | (uint16_t) rxPacket.channels[20] << 6;
            break;
        case 16: pwm = (uint16_t) rxPacket.channels[20] >> 5 | (uint16_t) rxPacket.channels[21] << 3;
            break;
        case 17: if (rxPacket.channel17) {
                pwm = 2047;
            } else {
                pwm = 0;
            }
            break;
        case 18: if (rxPacket.channel18) {
                pwm = 2047;
            } else {
                pwm = 0;
            }
            break;
        default: pwm = 0;
    }
    pwm &= 0x7ff;
    return (int16_t) pwm;
}