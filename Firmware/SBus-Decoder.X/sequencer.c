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
uint8_t currentInputValue = INPUT_NONE;

enum {
    SEQ_IDLE = 0, SEQ_RUNNING = 1, SEQ_DELAY = 2, SEQ_MOVE = 3
};
uint8_t seqState = SEQ_IDLE;
uint8_t seqCurrentStep = 0;
uint8_t seqNumSteps = 0;
volatile int16_t seqCountdown = 0; 
SequenceStep *pStep;          
int16_t lastServoPos[NUM_OUTPUTS + 1] = {-1, -1, -1, -1, -1};  //Need extra since outputs are 1 based
int16_t servoIncrement;
volatile int16_t moveServoPos;
uint8_t activeOutput;
int16_t finalPos;
uint8_t updateMove = 0;

void initSequencer(void) {
    initSequencerServos();
    if (settings.seqInputType == SBUS) {
        initSBusInput();
    } else {
        initPWMinput();
    }
    T4CONbits.ON = 0;
    T4CONbits.CKPS = 0b101; //1:32 pre-scale
    T4CONbits.OUTPS = 0; //1:1 post-scale
    T4HLTbits.CKSYNC = 1;
    T4HLTbits.PSYNC = 1;
    T4HLTbits.MODE = 0;
    T4CLKCON = 0b0110; //MFINTOSC at 31.25 kHz
    T4TMR = 0;
    T4PR = 49; //should give ~0.1s 49 is 0.05s
    IPR10bits.TMR4IP = 1;
    PIR10bits.TMR4IF = 0;
    PIE10bits.TMR4IE = 1;
    seqState = SEQ_IDLE;
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
    if (seqState == SEQ_IDLE && currentInputValue != sequencerInput) {
        currentInputValue = sequencerInput;
        seqCurrentStep = 0;
        seqState = SEQ_RUNNING;
        if (currentInputValue == INPUT_LOW) {
            seqNumSteps = settings.numLowSteps;
            pStep = settings.lowSteps;
        } else {
            seqNumSteps = settings.numHighSteps;
            pStep = settings.highSteps;
        }
        ledOn();
    }
    //State machine
    if (seqState == SEQ_RUNNING) {
        if (seqCurrentStep == seqNumSteps) {
            seqState = SEQ_IDLE;
            ledOff();
        } else {
            if (pStep->type == DELAY) {
                seqState = SEQ_DELAY;
                seqCountdown = (int16_t)(pStep->time * 2);
                T4TMR = 0;
                T4CONbits.ON = 1;
            } else {
                activeOutput = pStep->output;
                finalPos = pStep->position;
                if (pStep->time == 0 || lastServoPos[activeOutput] < 0) {
                    //TODO if first move put in a delay instead?
                    setServo(activeOutput, (uint16_t)finalPos);
                    lastServoPos[activeOutput] = finalPos;
                } else {
                    seqCountdown = (int16_t)(pStep->time * 2);
                    servoIncrement = (finalPos - lastServoPos[activeOutput])
                            / (seqCountdown + 1);
                    moveServoPos = lastServoPos[activeOutput] + servoIncrement;
                    setServo(activeOutput, (uint16_t)moveServoPos);
                    seqState = SEQ_MOVE;
                    T4TMR = 0;
                    T4CONbits.ON = 1;
                }
            }
            ++seqCurrentStep;
            ++pStep;
        }
    } else if (seqState == SEQ_DELAY) {
        if (seqCountdown == 0) {
            seqState = SEQ_RUNNING;
        }
    } else if (seqState == SEQ_MOVE) {
        if (seqCountdown == 0) {
            setServo(activeOutput, (uint16_t)finalPos);
            lastServoPos[activeOutput] = finalPos;
            seqState = SEQ_RUNNING;
        } else if (updateMove) {
            updateMove = 0;
            setServo(activeOutput, (uint16_t)moveServoPos);
        }
    }
}

void __interrupt(irq(TMR4), high_priority, base(8)) TMR4ISR(void) {
    --seqCountdown;
    if (seqCountdown == 0) {
        T4CONbits.ON = 0;
    } else if (seqState == SEQ_MOVE) {
        moveServoPos += servoIncrement;
        updateMove = 1;
    }
    PIR10bits.TMR4IF = 0;
}