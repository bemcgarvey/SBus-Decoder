#ifndef SBUS_SETTINGS_H
#define SBUS_SETTINGS_H

#include <stdint.h>

#define NUM_OUTPUTS 4

enum FrameRates {
    FRAME_50HZ = 1, FRAME_100HZ = 2, FRAME_150HZ = 3
};

enum FailsafeModes {
    FAIL_LAST = 1, FAIL_OFF = 2, FAIL_NEUTRAL = 3
};

enum OperatingModes {
    INITIALIZING = 0, SBUS_DECODER = 1, SERVO_SEQUENCER = 2, SERIAL_CONNECTED = 3, RANDOMIZER = 5
};

enum OptionFlags {SBUS_PASSTHROUGH4 = 0x01, SBUS_PASSTHROUGH3 = 0x02, REV_OUT1 = 0x04, REV_OUT2 = 0x08,
                 REV_OUT3 = 0x10, REV_OUT4 = 0x20};

enum StepTypes {
    SERVO = 0, DELAY = 1
};

enum SequencerInputTypes {
    SBUS = 0, PWM = 1
};

#define MAX_SEQUENCE_STEPS  10

typedef struct {
    uint8_t type;
    uint8_t output;
    uint16_t position;
    uint16_t time;
} SequenceStep;

typedef struct {
    uint8_t requestedMode;
    uint8_t options;
    struct {
        uint8_t channel;
        uint8_t frameRate;
        uint8_t failsafeMode;
        uint8_t reverse;
        int16_t subTrim;
    } outputs[NUM_OUTPUTS];
    uint8_t inputType;
    uint8_t numLowSteps;
    uint8_t numHighSteps;
    uint8_t sBusChannel;
    uint16_t revSubTrim;
    uint16_t random_min_servo;
    uint16_t random_max_servo;
    uint16_t random_min_time;
    uint16_t random_max_time;
    uint16_t random_min_move;
    uint16_t random_max_move;
    SequenceStep lowSteps[MAX_SEQUENCE_STEPS];
    SequenceStep highSteps[MAX_SEQUENCE_STEPS];
} Settings;

uint8_t calcChecksum(uint8_t *buff);

#endif // SBUS_SETTINGS_H
