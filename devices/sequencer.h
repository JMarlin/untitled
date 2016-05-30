#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <inttypes.h>

//Sequencer actions
#define SEQ_ACTON  0x01
#define SEQ_ACTOFF 0x02

//Sequencer notes
#define NOTE_A1  0
#define NOTE_AS1 1
#define NOTE_BF1 1
#define NOTE_B1  2
#define NOTE_C2  3
#define NOTE_CS2 4
#define NOTE_DF2 4
#define NOTE_D2  5
#define NOTE_DS2 6
#define NOTE_EF2 6
#define NOTE_E2  7
#define NOTE_F2  8
#define NOTE_FS2 9
#define NOTE_GF2 9
#define NOTE_G2  10
#define NOTE_GS2 11
#define NOTE_AF2 11
#define NOTE_A2  12
#define NOTE_AS2 13
#define NOTE_BF2 13
#define NOTE_B2  14
#define NOTE_C3  15
#define NOTE_CS3 16
#define NOTE_DF3 16
#define NOTE_D3  17
#define NOTE_DS3 18
#define NOTE_EF3 18
#define NOTE_E3  19
#define NOTE_F3  20
#define NOTE_FS3 21
#define NOTE_GF3 21
#define NOTE_G3  22
#define NOTE_GS3 23
#define NOTE_AF3 23
#define NOTE_A3  24
#define NOTE_AS3 25
#define NOTE_BF3 25
#define NOTE_B3  26
#define NOTE_C4  27
#define NOTE_CS4 28
#define NOTE_DF4 28
#define NOTE_D4  29
#define NOTE_DS4 30
#define NOTE_EF4 30
#define NOTE_E4  31
#define NOTE_F4  32
#define NOTE_FS4 33
#define NOTE_GF4 33
#define NOTE_G4  34
#define NOTE_GS4 35
#define NOTE_AF4 35
#define NOTE_A4  36
#define NOTE_AS4 37
#define NOTE_BF4 37
#define NOTE_B4  38
#define NOTE_C5  39
#define NOTE_CS5 40
#define NOTE_DF5 40
#define NOTE_D5  41
#define NOTE_DS5 42
#define NOTE_EF5 42
#define NOTE_E5  43
#define NOTE_F5  44
#define NOTE_FS5 45
#define NOTE_GF5 45
#define NOTE_G5  46
#define NOTE_GS5 47
#define NOTE_AF5 47
#define NOTE_A5  48
#define NOTE_AS5 49
#define NOTE_BF5 49
#define NOTE_B5  50
#define NOTE_C6  51
#define NOTE_CS6 52
#define NOTE_DF6 52
#define NOTE_D6  53
#define NOTE_DS6 54
#define NOTE_EF6 54
#define NOTE_E6  55
#define NOTE_F6  56
#define NOTE_FS6 57
#define NOTE_GF6 57
#define NOTE_G6  58
#define NOTE_GS6 59
#define NOTE_AF6 59
#define NOTE_A6  60
#define NOTE_AS6 61
#define NOTE_BF6 61
#define NOTE_B6  62
#define NOTE_C7  63
#define NOTE_CS7 64
#define NOTE_DF7 64
#define NOTE_D7  65
#define NOTE_DS7 66
#define NOTE_EF7 66
#define NOTE_E7  67
#define NOTE_F7  68
#define NOTE_FS7 69
#define NOTE_GF7 69
#define NOTE_G7  70
#define NOTE_GS7 71
#define NOTE_AF7 71
#define NOTE_A7  72
#define NOTE_AS7 73
#define NOTE_BF7 73
#define NOTE_B7  74
#define NOTE_C8  75
#define NOTE_CS8 76
#define NOTE_DF8 76
#define NOTE_D8  77
#define NOTE_DS8 78
#define NOTE_EF8 78
#define NOTE_E8  79
#define NOTE_F8  80
#define NOTE_FS8 81
#define NOTE_GF8 81
#define NOTE_G8  82
#define NOTE_GS8 83
#define NOTE_AF8 83


typedef struct SequenceMessage_s {
    uint8_t note;
    uint8_t action;
} SequenceMessage;

typedef struct SequenceTimingNode_s{
    float event_time;
    struct SequenceTimingNode_s* prev;
    struct SequenceTimingNode_s* next;
    SequenceMessage* message;
} SequenceTimingNode;

typedef struct SequenceTimingList_s {
    float current_time;
    unsigned node_count;
    SequenceTimingNode* root_node;
} SequenceTimingList;

typedef struct SequenceMessageCollection_s {
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
