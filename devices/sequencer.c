#include "sequencer.h"
#include "../config.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

void delete_timing_node(SequenceTimingNode* timing_node) {

    //We don't delete the message because it may have been handed to something else
    
    free(timing_node);
}

void delete_timing_node_and_child(SequenceTimingNode* timing_node) {

    free(timing_node->message);
    delete_timing_node(timing_node);
}

void delete_timing_list(SequenceTimingList* timing_list) {
    
    SequenceTimingNode* temp_node;

    while(timing_list->root_node) {
    
        temp_node = timing_list->root_node->next;
        delete_timing_node_and_child(timing_list->root_node);
        timing_list->root_node = temp_node;
    }

    free(timing_list);
}

int standard_sequence_generator(SequenceMessageCollection* messages, SequenceTimingList* timing_list) {

    //Guard against null pointers
    if(!messages)
        return 0;

    if(!timing_list) {
  
        messages->message_count = 0;
        return 0;
    }

    //If the list count hit zero, we're done generating
    if(timing_list->node_count == 0) {
        
        //printf("Sequencer finished\n");
        messages->message_count = 0;
        return 0;
    }

    int count;
    SequenceTimingNode* current_node = timing_list->root_node;
    
    //Count the number of events in the list with times below or equal to the current
    //playback time 
    for(count = 0; current_node && current_node->event_time <= timing_list->current_time; count++)
        current_node = current_node->next;

    messages->message_count = count;

    //Allocate a message array to recieve the consumed messages
    if(count) {

        printf("Sequencer encountered an event at time %f\n", timing_list->current_time);

        messages->item = (SequenceMessage**)malloc(count * sizeof(SequenceMessage*));
        
        if(!messages->item) {

            delete_timing_list(timing_list);
            messages->message_count = 0; 
            return 0;
        }

        //Move the message references from the first [count] nodes into 
        //the allocated message array and drop those nodes from the list
        for(count = 0; count < messages->message_count; count++) {
        
            messages->item[count] = timing_list->root_node->message;
            SequenceTimingNode* dead_node = timing_list->root_node; 
            timing_list->root_node = timing_list->root_node->next;
            delete_timing_node(dead_node); 
        }

        //Make sure the root doesn't have a prev value
        if(timing_list->root_node)
            timing_list->root_node->prev = (SequenceTimingNode*)0;

        //Update the number of items in the list
        timing_list->node_count -= count;
    }
  
    //Messages consumed and passed along properly
    timing_list->current_time += MS_PER_SAMPLE;
    return 1; 
}

SequenceTimingList* new_timing_list() {

    SequenceTimingList* timing_list = (SequenceTimingList*)malloc(sizeof(SequenceTimingList));

    if(!timing_list)
        return timing_list;

    timing_list->current_time = 0.0;
    timing_list->node_count = 0;
    timing_list->root_node = (SequenceTimingNode*)0;

    return timing_list;
}

Sequencer* new_sequencer() {

    Sequencer* sequencer = (Sequencer*)malloc(sizeof(Sequencer));

    if(!sequencer)
        return sequencer;

    sequencer->generator = standard_sequence_generator;
    sequencer->timing_list = new_timing_list();

    if(!sequencer->timing_list) {
    
        free(sequencer);
        return (Sequencer*)0;
    }

    return sequencer;
}

SequenceMessage* new_sequence_message(float note, uint8_t action) {
    
    SequenceMessage* message = (SequenceMessage*)malloc(sizeof(SequenceMessage));
    
    if(!message)
        return message;
        
    message->note = note;
    message->action = action;
    
    return message;
}

SequenceTimingNode* new_timing_node(float time, float note, uint8_t action) {

    SequenceTimingNode* new_node = (SequenceTimingNode*)malloc(sizeof(SequenceTimingNode));

    new_node->event_time = time;
    new_node->message = new_sequence_message(note, action);

    if(!new_node->message) {

        free(new_node);
        return (SequenceTimingNode*)0;
    }

    new_node->prev = (SequenceTimingNode*)0;
    new_node->next = (SequenceTimingNode*)0;

    return new_node;
}

void sequencer_add_event(Sequencer* sequencer, float time, float note, uint8_t action) {

    SequenceTimingNode* new_node = new_timing_node(time, note, action);
    
    if(!new_node)
        return;

    //At the moment we'll let this fail silently as with the allocation of the
    //new node
    if(sequencer->timing_list->root_node) {
    
        SequenceTimingNode* current_node = sequencer->timing_list->root_node;

        //Fast-forward until we either hit the end of the list or we find a node
        //that is further in the future than us
        while(current_node->next && current_node->event_time < new_node->event_time)
            current_node = current_node->next;

        //Either append the node to the end of the list or insert it before the 
        //current node
        if(current_node->event_time < new_node->event_time) {
            
            new_node->prev = current_node;
            current_node->next = new_node;
        } else {
        
            new_node->next = current_node;
            new_node->prev = current_node->prev;
            
            if(current_node->prev)
                current_node->prev->next = new_node;
            else //If inserting before the root node, we must make sure that we re-point the root node
                sequencer->timing_list->root_node = new_node;

            current_node->prev = new_node;
        }
    } else {

        sequencer->timing_list->root_node = new_node;
    }

    sequencer->timing_list->node_count += 1;
}

int sequencer_pull_next_sample(Sequencer* sequencer, SequenceMessageCollection* messages) {
    
    //Allows for funky generators, like arpeggiators
    return sequencer->generator(
               messages,
               sequencer->timing_list
           ); 
}
