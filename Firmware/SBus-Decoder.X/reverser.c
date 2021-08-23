/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: reverser.c                                //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Servo reverser functions           //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "reverser.h"
#include "settings.h"
#include "pwm.h"
#include "sbus.h"
#include "servo.h"

int16_t calcReverseValue(int16_t value);

void initReverser(void) {
    initReverserServos();
    if (settings.inputType == SBUS) {
        initSBusInput();
    } else {
        initPWMinput();
    }
}

void reverserTasks(void) {
    int16_t newValue = 0;
    bool update = false;
    if (settings.inputType == SBUS) {
        if (packetUpdate) {
            packetUpdate = false;
            newValue = calcReverseValue(decodeChannel(settings.sBusChannel));
            update = true;
        }
    } else if (pulseUpdate) {
        pulseUpdate = false;
        newValue = calcReverseValue(pwmPulse);
        update = true;
    }
    if (update) {
        //Only enabled outputs will actually get set
        setServo(1, (uint16_t)newValue);
        setServo(2, (uint16_t)newValue);
        setServo(3, (uint16_t)newValue);
        setServo(4, (uint16_t)newValue);
    }
}

int16_t calcReverseValue(int16_t value) {
    value = 2047 - value;
    value += settings.revSubTrim;
    if (value < 0) {
        value = 0;
    } else if (value > 2047) {
        value = 2047;
    }
    return value;
}