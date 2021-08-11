/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: settings.c                                //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Settings functions                 //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include "settings.h"
#include "string.h"

#define EE_START_ADDRESS 0

Settings settings = {
    0,
    {{0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}}
};

const Settings defaultSettings = {
    SBUS_DECODER,
    {{0, FRAME_50HZ, FAIL_LAST},
    {0, FRAME_50HZ, FAIL_LAST},
    {0, FRAME_50HZ, FAIL_LAST},
    {0, FRAME_50HZ, FAIL_LAST}}
};

void writeEE(uint16_t address, uint8_t* bytes, uint16_t count);
void readEE(uint16_t address, uint8_t* bytes, uint16_t count);

bool loadSettings(void) {
    uint8_t chksum;
    uint16_t eeAddress = EE_START_ADDRESS;
    readEE(eeAddress, (uint8_t *) &settings, sizeof (Settings));
    eeAddress += sizeof (Settings);
    readEE(eeAddress, &chksum, 1);
    if ((calcChecksum((uint8_t *)&settings) ^ chksum) == 0) {
        return true;
    } else {
        return false;
    }
}

bool saveSettings(void) {
    uint8_t chksum;
    chksum = calcChecksum((uint8_t *)&settings);
    uint16_t eeAddress = EE_START_ADDRESS;
    writeEE(eeAddress, (uint8_t *)&settings, sizeof (Settings));
    eeAddress += sizeof (Settings);
    writeEE(eeAddress, &chksum, 1);
    eeAddress = EE_START_ADDRESS;
    Settings tempSettings;
    readEE(eeAddress, (uint8_t *) &tempSettings, sizeof (Settings));
    if (memcmp(&settings, &tempSettings, sizeof (Settings)) != 0) {
        return false;
    }
    eeAddress += sizeof (Settings);
    uint8_t byte;
    readEE(eeAddress, &byte, 1);
    if (byte != chksum) {
        return false;
    }
    return true;
}

bool setDefaultSettings(void) {
    memcpy(&settings, &defaultSettings, sizeof(Settings));
    return saveSettings();
}

void writeEE(uint16_t address, uint8_t* bytes, uint16_t count) {
    NVMADRU = 0x38;
    NVMADRH = (uint8_t) ((address & 0xFF00) >> 8);
    NVMADRL = (uint8_t) (address & 0x00FF);
    NVMCON1bits.NVMCMD = 0b100; //write and post increment
    for (int i = 0; i < count; ++i) {
        NVMDATL = *bytes++;
        NVMLOCK = 0x55;
        NVMLOCK = 0xAA;
        NVMCON0bits.GO = 1; // Start byte write
        while (NVMCON0bits.GO);
    }
}

void readEE(uint16_t address, uint8_t* bytes, uint16_t count) {
    NVMADRU = 0x38;
    NVMADRH = (uint8_t) ((address & 0xFF00) >> 8);
    NVMADRL = (uint8_t) (address & 0x00FF);
    NVMCON1bits.NVMCMD = 0b001; //read and post increment
    for (int i = 0; i < count; ++i) {
        NVMCON0bits.GO = 1;
        *bytes = NVMDATL;
        ++bytes;
    }
}

uint8_t calcChecksum(uint8_t *buff) {
    uint8_t len = sizeof(Settings);
    uint8_t chksum = 0xaa;
    while (len > 0) {
        chksum ^= *buff;
        ++buff;
        --len;
    }
    return chksum;
}