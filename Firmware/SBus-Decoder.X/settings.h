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

enum {
    FRAME_50HZ = 1, FRAME_100HZ = 2, FRAME_150HZ = 3
};

enum {
    FAIL_LAST = 1, FAIL_OFF = 2, FAIL_NEUTRAL = 3
};

enum {
    INITIALIZING = 0, SBUS_DECODER = 1, SERVO_SEQUENCER = 2, SERIAL_CONNECTED = 3
};

//TODO add reverse setting to each channel - allows two servos on same channel with one reversed
//TODO add sub-trim

typedef struct {
    uint8_t requestedMode;
    struct {
        uint8_t channel;
        uint8_t frameRate;
        uint8_t failsafeMode;
    } outputs[NUM_OUTPUTS];
} Settings;

extern Settings settings;

bool loadSettings(void);
bool saveSettings(void);
bool setDefaultSettings(void);
uint8_t calcChecksum(uint8_t *buff);

#endif	/* SETTINGS_H */

