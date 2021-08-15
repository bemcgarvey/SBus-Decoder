/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: main.c                                    //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: main loop and ISR                  //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "serial.h"
#include "sbus.h"
#include "led.h"
#include "timers.h"
#include "servo.h"
#include "settings.h"

void lockPPS(void);
void configInterrupts(void);
void configPMD(void);

//TODO Use PMD to turn off unused modules
//TODO Final warning check

bool failsafeEngaged = 0;

void main(void) {
    uint8_t mode = INITIALIZING;
    if (PCON0bits.RWDT == 0) {
        mode = WDT;
    }
    PCON0bits.RWDT = 1;
    OSCTUNE = 0;
    //All pins digital outputs except RC3
    //RC3 needs to stay in input so it doesn't detect as serial when in
    //pass-through mode.
    ANSELA = 0;
    LATA = 0;
    TRISA = 0;
    ANSELC = 0;
    LATC = 0;
    TRISC = 0b00001000;
    initLED();
    if (mode != WDT) {
        if (!loadSettings()) {
            if (!setDefaultSettings()) {
                while (1) {
                    ledToggle();
                    __delay_ms(200);
                }
            }
        }
        if (detectSerial()) {
            mode = SERIAL_CONNECTED;
            ledOn();
            initSerial();
            while (1) {
                serialTasks();
            }
        }
    }
    mode = settings.requestedMode;
    configPMD();
    configInterrupts();
    initServos();
    initSBus();
    initTimer2();
    lockPPS();
    int packetCount = 0;
    WDTCON0bits.PS = 0b00101; //Watchdog timer = 32ms
    WDTCON0bits.SEN = 1;
    while (1) {
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
        CLRWDT();
    }
}

void lockPPS(void) {
    PPSLOCK = 0x55;
    PPSLOCK = 0xaa;
    PPSLOCKbits.PPSLOCKED = 1;
    //Lock priority for good while were at it
    PRLOCK = 0x55;
    PRLOCK = 0xAA;
    PRLOCKbits.PRLOCKED = 1;
}

void configInterrupts(void) {
    INTCON0bits.IPEN = 1;
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0; // unlock IVT
    IVTBASEU = 0;
    IVTBASEH = 0;
    IVTBASEL = 8;
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 1; // lock IVT
    INTCON0bits.GIEH = 1;
    INTCON0bits.GIEL = 1;
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
