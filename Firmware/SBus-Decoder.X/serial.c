/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: serial.c                                  //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: serial functions (non-sBus)        //
/////////////////////////////////////////////////////

#include <xc.h>
#include "serial.h"
#include "timers.h"
#include "version.h"
#include "settings.h"
#include "string.h"
#include "servo.h"

void txBytes(uint8_t *buff, uint8_t count);

bool detectSerial(void) {
    WPUCbits.WPUC1 = 1;
    TRISCbits.TRISC1 = 1;
    __delay_ms(1);
    if (PORTCbits.RC1 == 0) {
        LATCbits.LATC1 = 0;
        TRISCbits.TRISC1 = 0;
        WPUCbits.WPUC1 = 0;
        return true;
    } else {
        TRISCbits.TRISC1 = 0;
        WPUCbits.WPUC1 = 0;
        return false;
    }
}

void initSerial(void) {
    TRISAbits.TRISA0 = 1; //RX
    TRISCbits.TRISC0 = 0; //TX
    LATAbits.LATA1 = 0; //Set to 0 for gnd
    TRISAbits.TRISA1 = 0; //Gnd
    U2RXPPS = 0b00000; //RA0
    RC0PPS = 0x13; //UART2TX
    U2CON1bits.ON = 0;
    U2CON0bits.BRGS = 1;
    U2CON0bits.TXEN = 1;
    U2CON0bits.RXEN = 1;
    U2CON0bits.MODE = 0; //8 bit no parity
    U2BRG = 138; //115200 baud
    U2CON1bits.ON = 1;
    initSerialServo();
}

void serialTasks(void) {
    uint8_t rx;
    Settings tempSettings;
    uint8_t len;
    uint8_t *p;
    uint8_t chksum;
    uint16_t servoValue; 
    while (PIR8bits.U2RXIF == 0);
    rx = U2RXB;
    switch (rx) {
        case 'v':
            while (!PIR8bits.U2TXIF);
            U2TXB = MAJOR_VERSION;
            while (!PIR8bits.U2TXIF);
            U2TXB = MINOR_VERSION;
            break;
        case 's':
            txBytes((uint8_t *) & settings, sizeof (Settings));
            chksum = calcChecksum((uint8_t *)&settings);
            txBytes(&chksum, 1);
            break;
        case 'u':
            len = sizeof (Settings);
            p = (uint8_t *)&tempSettings;
            while (len > 0) {
                while (!PIR8bits.U2RXIF);
                *p++ = U2RXB;
                --len;
            }
            while (!PIR8bits.U2RXIF);
            uint8_t chksum = U2RXB;
            char ackVal = NACK;
            if (chksum == calcChecksum((uint8_t *)&tempSettings)) {
                memcpy(&settings, &tempSettings, sizeof (Settings));
                if (saveSettings()) {
                    ackVal = ACK;
                }
            }
            while (!PIR8bits.U2TXIF);
            U2TXB = ackVal;
            break;
        case 't':
            while (!PIR8bits.U2RXIF);
            rx = U2RXB;
            while (!PIR8bits.U2RXIF);
            servoValue = U2RXB;
            servoValue <<= 8;
            servoValue |= rx;
            PWMEN = 0b100;
            setServo(4, servoValue);
            break;
        case 'x':
            PWMEN = 0;
            break;
    }
}

void txBytes(uint8_t *buff, uint8_t count) {
    while (count > 0) {
        while (!PIR8bits.U2TXIF);
        U2TXB = *buff++;
        --count;
    }
}

