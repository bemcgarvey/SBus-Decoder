/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: main.c                                    //
// Target: PIC16F18313                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: main loop                          //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#define _XTAL_FREQ   4000000U

void configPins(void);
void configInterrupts(void);
void configPMD(void);

void main(void) {
    OSCCON1bits.NDIV = 0;
    OSCFRQ = 3;
    OSCTUNE = 0;
    configPins();
    configPMD();
    configInterrupts();
    while (1) {
        LATAbits.LATA2 = 1;
        __delay_ms(1000);
        LATAbits.LATA2 = 0;
        __delay_ms(1000);
    }
}

void configPins(void) {
    ANSELA = 0;
    LATA = 0b00000000;
    TRISA = 0b00000000;
    PPSLOCK = 0x55;
    PPSLOCK = 0xaa;
    PPSLOCKbits.PPSLOCKED = 1;
}

void configInterrupts(void) {
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void configPMD(void) {
    /*
    PMD0 = 0b01000111;
    PMD1 = 0b10000001;
    PMD2 = 0b01100110;
    PMD3 = 0b01110011;
    PMD4 = 0b00100010;
    PMD5 = 0b00000111;
     */
}

void __interrupt() isr(void) {

}