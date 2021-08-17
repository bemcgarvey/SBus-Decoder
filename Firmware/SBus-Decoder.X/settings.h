/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: settings.h                                //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Settings functions                 //
/////////////////////////////////////////////////////

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_OUTPUTS 4

enum FrameRates {
    FRAME_50HZ = 1, FRAME_100HZ = 2, FRAME_150HZ = 3
};

enum FailsafeModes {
    FAIL_LAST = 1, FAIL_OFF = 2, FAIL_NEUTRAL = 3
};

enum ModeType {
    INITIALIZING = 0, SBUS_DECODER = 1, SERVO_SEQUENCER = 2, SERIAL_CONNECTED = 3, WDT = 99
};

enum StepType {
    SERVO = 0, DELAY = 1
};

enum SequencerInputType {
    SBUS = 0, PWM = 1
};

#define MAX_SEQUENCE_STEPS  10

typedef struct {
    uint8_t type;
    uint8_t output;
    int16_t position;
    uint16_t time;
} SequenceStep;

typedef struct {
    uint8_t requestedMode;

    struct {
        uint8_t sbusPassthrough4 : 1;
        uint8_t sbusPassthrough3 : 1;
        uint8_t : 6;
    } options;

    struct {
        uint8_t channel;
        uint8_t frameRate;
        uint8_t failsafeMode;
        uint8_t reverse;
        int16_t subTrim;
    } outputs[NUM_OUTPUTS];
    uint8_t seqInputType;
    uint8_t numLowSteps;
    uint8_t numHighSteps;
    uint8_t seqSbusChannel;
    SequenceStep lowSteps[MAX_SEQUENCE_STEPS];
    SequenceStep highSteps[MAX_SEQUENCE_STEPS];
} Settings;

extern Settings settings;

bool loadSettings(void);
bool saveSettings(void);
bool setDefaultSettings(void);
uint8_t calcChecksum(uint8_t *buff);

#endif	/* SETTINGS_H */

