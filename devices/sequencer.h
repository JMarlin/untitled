#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <inttypes.h>

//Sequencer actions
#define SEQ_ACTON  0x01
#define SEQ_ACTOFF 0x02

typedef struct SequenceMessage {
    uint8_t note;
    uint8_t action;
} SequenceMessage;

typedef struct SequenceTimingNode {
    float event_time;
    struct SequenceTimingNode* prev;
    struct SequenceTimingNode* next;
    SequenceMessage* message;
} SequenceTimingNode;

typedef struct SequenceTimingList {
    float current_time;
    unsigned node_count;
    SequenceTimingNode* root_node;
} SequenceTimingList;

typedef struct SequenceMessageCollection {
    unsigned message_count;
    SequenceMessage** item;
} SequenceMessageCollection;

//A sequence generator function takes a seqence timing list and outputs a collection of messages that
//are occurring along with an int indicating if the sequencer is still running
typedef int(SequenceGeneratorFunction)(SequenceMessageCollection*, SequenceTimingList*);

typedef struct Sequencer {
    SequenceGeneratorFunction* generator;
    SequenceTimingList* timing_list;
} Sequencer;

Sequencer* new_sequencer(void);
void sequencer_add_event(Sequencer* sequencer, float time, uint8_t note, uint8_t action);
int sequencer_pull_next_sample(Sequencer* sequencer, SequenceMessageCollection* messages);

#endif //SEQUENCER_H