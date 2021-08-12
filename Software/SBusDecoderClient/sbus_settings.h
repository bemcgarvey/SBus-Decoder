#ifndef SBUS_SETTINGS_H
#define SBUS_SETTINGS_H

#include <stdint.h>

#define NUM_OUTPUTS 4

enum {
    FRAME_50HZ = 1, FRAME_100HZ = 2, FRAME_150HZ = 3
};

enum {
    FAIL_LAST = 1, FAIL_OFF = 2, FAIL_NEUTRAL = 3
};

enum {
    INITIALIZING = 0, SBUS_DECODER = 1, SERVO_SEQUENCER = 2, SERIAL_CONNECTED = 3
};

typedef struct {
    uint8_t requestedMode;
    uint8_t filler;
    struct {
        uint8_t channel;
        uint8_t frameRate;
        uint8_t failsafeMode;
        uint8_t reverse;
        int16_t subTrim;
    } outputs[NUM_OUTPUTS];
} Settings;

uint8_t calcChecksum(uint8_t *buff);

#endif // SBUS_SETTINGS_H
