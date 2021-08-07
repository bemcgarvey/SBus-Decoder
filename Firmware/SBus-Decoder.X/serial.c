#include <xc.h>
#include "serial.h"
#include "timers.h"

bool detectSerial(void) {
    ANSELC = 0;
    ANSELA = 0;
    WPUCbits.WPUC1 = 1;
    TRISCbits.TRISC1 = 1;
    __delay_ms(1);
    if (PORTCbits.RC1 == 0) {
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
    RC0PPS = 0x13;     //UART2TX
    U2CON1bits.ON = 0;
    U2CON0bits.BRGS = 1;
    U2CON0bits.TXEN = 1;
    U2CON0bits.RXEN = 1;
    U2CON0bits.MODE = 0; //8 bit no parity
    U2BRG = 138;  //115200 baud
    
    U2CON1bits.ON = 1;
}

void putch(char c) {
    while (!PIR8bits.U2TXIF);
    U2TXB = c;
}