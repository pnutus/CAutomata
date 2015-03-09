//
//  main.cpp
//  CellularAutomata
//
//  Created by Pontus Granström on 2015-02-13.
//  Copyright (c) 2015 Pontus Granström. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "pcg_basic.h"

typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;
typedef uint8_t u8;
typedef uint32_t bool32;

#define CHUNK_SIZE 64

struct ca_state {
    int bit_count;
    u64* bits;
};

void print_state(ca_state state)
{
    // TODO: Make this two loops instead of divmod every digit
    char string[state.bit_count + 1];
    for (int bit_index = 0;
         bit_index < state.bit_count;
         bit_index++)
    {
        u64 state_chunk = state.bits[bit_index / CHUNK_SIZE];
        int chunk_index = bit_index % CHUNK_SIZE;
        bool32 active = ((state_chunk >> chunk_index) & 1);
        char character = active * '#' + (1 - active) * ' ';
        string[state.bit_count - bit_index - 1] = character;
    }
    string[state.bit_count] = 0;
    printf("%s\n", string);
}

int string_length(const char* string)
{
    int result = 0;
    while (*string++)
    {
        result++;
    }
    return result;
}

ca_state init_state(int bit_count)
{
    ca_state state;
    state.bit_count = bit_count;
    state.bits = (u64*) malloc((state.bit_count + 64) / 8);
    return state;
}


ca_state state_from_string(const char* string)
{
    ca_state state = init_state(string_length(string));
    for (int bit_index = 0;
         bit_index < state.bit_count;
         bit_index++)
    {
        u64 chunk = state.bits[bit_index / CHUNK_SIZE];
        chunk = (chunk << 1) | (string[bit_index] != ' ');
        state.bits[bit_index / CHUNK_SIZE] = chunk;
    }
    return state;
}

ca_state evolve_ca(ca_state parent_state, u8 rule, ca_state child_state)
{
    memset(child_state.bits, 0, (child_state.bit_count + 64) / 8);
    u64 right_edge = (pcg32_random() & 1);
    child_state.bits[0] = right_edge;
    
    u32* parent_half_chunks = (u32*) parent_state.bits;
    u32* child_half_chunks  = (u32*) child_state.bits;
    
    for (int half_chunk_index = 0;
         half_chunk_index < ((parent_state.bit_count / (CHUNK_SIZE/2)) + 1);
         half_chunk_index++)
    {
        u64 parent_chunk = *((u64*) (parent_half_chunks++));
        u64 child_chunk = 0;
        
        for (int bit_index = 0;
             bit_index < (CHUNK_SIZE / 2);
             bit_index++)
        {
            if ((half_chunk_index*CHUNK_SIZE/2 + bit_index + 2) >= parent_state.bit_count)
            {
                u64 left_edge = (pcg32_random() & 1) << (bit_index + 1);
                child_chunk |= left_edge;
                break;
            }
            
            u8 three_mask = 7; // 111
            u8 three_state = (parent_chunk >> bit_index) & three_mask;
            u64 active = ((rule >> three_state) & 1);
            child_chunk |= (active << (bit_index + 1));
        }
        u64* child_chunk_pointer = (u64*) (child_half_chunks++);
        *child_chunk_pointer |= child_chunk;
        
    }
    return child_state;
}

int main(int argc, const char * argv[]) {
    if (argc > 1)
    {
        pcg32_srandom(time(0), 0);
        
        u8 rule = atoi(argv[1]);
        printf("R%d\n", rule);
        
        ca_state state;
        if (argc > 2)
        {
            state = state_from_string(argv[2]);
        }
        else
        {
            state = init_state(64);
            state.bits[0] = (((u64) pcg32_random()) << 32) | pcg32_random();
        }
        
        ca_state new_state = init_state(state.bit_count);
        
        for (;;)
        {
            print_state(state);
            evolve_ca(state, rule, new_state);
            
            ca_state temp = state;
            state = new_state;
            new_state = temp;
        }
    }
    else
    {
        puts("Please supply a rule (number between 0 and 255).");
    }
    return 0;
}
