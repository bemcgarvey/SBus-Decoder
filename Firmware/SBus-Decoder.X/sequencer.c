#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "sequencer.h"
#include "settings.h"
#include "servo.h"
#include "pwm.h"
#include "sbus.h"
#include "led.h"

volatile uint8_t sequencerInput = INPUT_NONE;

void initSequencer(void) {
    initSequencerServos();
    if (settings.seqInputType == SBUS) {
        initSBusInput();
    } else {
        initPWMinput();
    }
    
}

void sequencerTasks(void) {
    if (settings.seqInputType == SBUS) {
        if (packetUpdate) {
            packetUpdate = false;
            int16_t input = decodeChannel(settings.seqSbusChannel);
            if (input < SBUS_LOW) {
                sequencerInput = INPUT_LOW;   
            }
            if (input > SBUS_HIGH) {
                sequencerInput = INPUT_HIGH;
            }
        }
    }
    if (sequencerInput == INPUT_HIGH) {
        ledOn();
    } else {
        ledOff();
    }
}
