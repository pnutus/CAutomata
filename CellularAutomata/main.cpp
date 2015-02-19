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
#include <unistd.h>

typedef int64_t  s64;
typedef uint64_t u64;
typedef uint8_t u8;
typedef uint32_t bool32;

// TODO(Pontus): States of arbitrary length

void print_state(u64 state)
{
    char string[65] = {};
    for (int digit_index = 63;
         digit_index >= 0;
         digit_index--)
    {
        bool32 active = ((state >> digit_index) & 1);
        char character = active * '#' + (1 - active) * ' ';
        string[digit_index] = character;
    }
    printf("%s\n", string);
}

void ca(u8 rule, u64 initial_state)
{
    u64 parent_state = initial_state;
    u8 three_mask = 7; // 111
    for (;;)
    {
        print_state(parent_state);
        usleep(50000);
        
        u64 state = 0;
        for (int cell_index = 0;
             cell_index < 62;
             cell_index++)
        {
            u8 three_state = (parent_state >> cell_index) & three_mask;
            u64 active = ((rule >> three_state) & 1);
            state |= (active << (cell_index + 1));
        }
        
        u64 edges = ( ((random() & 1) << 63) | (random() & 1) );
        parent_state = state | edges;
    }
    return;
}

int main(int argc, const char * argv[]) {
    if (argc > 1)
    {
        srandomdev();
        u64 initial_state;
        u8 rule = atoi(argv[1]);
        if (argc > 2)
        {
            //Todo(Pontus): Parse this string as a bit string instead
            initial_state = atoll(argv[2]);
        }
        else
        {
            initial_state = (random() << 32) | random();
        }
        printf("R%d\n", rule);
        ca(rule, initial_state);
    }
    else
    {
        puts("Please supply a rule (number between 0 and 255).");
    }
    return 0;
}
