/* 
 * File:   sequencer.h
 * Author: bemcg
 *
 * Created on August 15, 2021, 6:27 PM
 */

#ifndef SEQUENCER_H
#define	SEQUENCER_H

enum {INPUT_NONE = 0, INPUT_LOW = 1, INPUT_HIGH = 2};
extern volatile uint8_t sequencerInput;

void initSequencer(void);
void sequencerTasks(void);

#endif	/* SEQUENCER_H */

