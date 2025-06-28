/////////////////////////////////////////////////////
// Project: SBus-Decoder                           //
// File: randomizer.c                              //
// Target: PIC18F0xQ40                             // 
// Compiler: XC8                                   //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Random servo output functions      //
/////////////////////////////////////////////////////

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "randomizer.h"
#include "settings.h"
#include "servo.h"
#include <stdlib.h>

static volatile uint16_t servoPos[NUM_OUTPUTS];
static volatile uint16_t nextMove[NUM_OUTPUTS];
static volatile uint16_t ticks = 0;
static uint8_t updateServos = 0;
static uint16_t moveRange;
static uint16_t timeRange;

void initRandomizer(void) {
    //TODO use temp sensor as seed value
    srand(1234);
    moveRange = settings.random_max_move - settings.random_min_move;
    timeRange = settings.random_max_time - settings.random_min_time;
    initRandomizerServos();
    for (uint8_t i = 0; i < NUM_OUTPUTS; ++i) {
        servoPos[i] = 1024;
        nextMove[i] = settings.random_min_time + ((uint16_t)rand() % timeRange);
    }
    updateServos = 0b1111; //force an update to starting values
    ticks = 0;
    T4CONbits.ON = 0;
    T4CONbits.CKPS = 0b101; //1:32 pre-scale
    T4CONbits.OUTPS = 0; //1:1 post-scale
    T4HLTbits.CKSYNC = 1;
    T4HLTbits.PSYNC = 1;
    T4HLTbits.MODE = 0;
    T4CLKCON = 0b0110; //MFINTOSC at 31.25 kHz
    T4TMR = 0;
    T4PR = 98; //~0.1s
    IPR10bits.TMR4IP = 1;
    PIR10bits.TMR4IF = 0;
    PIE10bits.TMR4IE = 1;
    T4CONbits.ON = 1;
}

void randomizerTasks(void) {
    if (updateServos) {
        uint8_t flag = 1;
        for (uint8_t i = 0; i < NUM_OUTPUTS; ++i) {
            if (updateServos & flag) {
                uint16_t move = settings.random_min_move + ((uint16_t)rand() % moveRange);
                if (rand() % 2) {
                    move = -move;
                }
                servoPos[i] += move;
                if (servoPos[i] < settings.random_min_servo) {
                    servoPos[i] = settings.random_min_servo;
                }
                else if (servoPos[i] > settings.random_max_servo) {
                    servoPos[i] = settings.random_max_servo;
                }
                setServo(i, servoPos[i]);
                nextMove[i] += settings.random_min_time + ((uint16_t)rand() % timeRange);
            }
            flag <<= 1;
        }
        updateServos = 0;
    }
}

void randomizerISR(void) {
    ++ticks;
    uint8_t flag = 1;
    for (uint8_t i = 0; i < NUM_OUTPUTS; ++i) {
        if (ticks >= nextMove[i]) {
            updateServos |= flag;
        }
        flag <<= 1;
    }
}