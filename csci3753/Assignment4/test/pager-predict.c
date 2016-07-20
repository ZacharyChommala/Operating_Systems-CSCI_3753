// Author: Derek Gorthy
// SID: 102359021
// References: Jack Dinkel, Luke Meszar, Izaak Weise, Alex Curtis
//
// Material from references: whiteboard discussions about the graph and flow of the program were had. These were mostly about
// the structure of the graph and how to predict which paths belonged to the five different programs given to us in
// psuedocode. The same basic testing strategy was used between several, so any probabilities were found in the same manner.
// However, we did determine these seperately. 
//
// Description: This is the predictive solution to the paging problem. I determined a basic graph of logical program flow
// to determine the current page's neighbors and its neighbors' neighbors. The graph below was constructed from testing
// a single process as it completed my LRU solution. From there, I merely made a chart of (to, from) and, after several
// rounds of testing, determined all possible paths.

// {(0,1),}
// {(1,2)}
// {(2,3)}
// {(3,0),(3,4),(3,10)}
// {(4,5)}
// {(5,6)}
// {(6,7)}
// {(7,8)}
// {(8,0),(8,9)}
// {(9,10)}
// {(10,11)}
// {(11,0),(11,12)}
// {(12,13)}
// {(13,0),(13,9),(13,14)}
// {(14,0)}

// I will explain more about my probability array where I declare it.

// Initially, my strategy was to only swap in the current page and all next pages. This resulted in nearly the same performance
// as LRU. It prompted me to investigate swapping in all pages after that jump - reducing my performance (this caused lots of 
// thrashing). 

/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 *  This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    /* Local vars */
    int pc;
    int page;
    int proc;
    int pos;
    int proc_count;
    int a;

// After determining what the graph looked like, I decided to see how often each path (if there are multiple) is taken. Honestly,
// this was mostly done by guessing based on how often I saw a jump. I tried to keep it simple and split up the probabilities into
// quarters with the exception of needing to use .125 to make the 14th row work. Those with .125 probabilities appeared much less
// often than paths with probability .25

static double pagePredict[15][15] = { // statically declared because 15 possible pages is constant
        {0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.25,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.0,0.25,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0},
        {0.5,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0},
        {0.25,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.75,0.0,0.0},
        {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0},
        {0.125,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.75,0.0,0.0,0.0,0.0,0.125},
        {1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
    };

    for(proc=0;proc<MAXPROCESSES;proc++){
        // Pages needing to be swapped in are tracked using this bit (actually int) array
        // All bits remaining 0 can safely be swapped out
        int bit_array[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            
            pc = q[proc].pc;
            page = pc/PAGESIZE;
            // set the page we currently need to 1
            bit_array[page] = 1;

            // Even if there is a low probability of the neighbor being next, add to bit array to reduce thrashing
            for(pos = 0; pos < 15; pos++){
                if(pagePredict[page][pos] > 0){
                    bit_array[pos] = 1;
                }
            }

            // Now check the neighbors of each neighbor
            // If first jump is low probability, don't swap in its neighbors
            for(pos = 0; pos < 15; pos++){
                if(pagePredict[page][pos] > .74){ // Any jump with probability with high likelyhood will pass this
                    for(a = 0; a < 15; a++){
                        if(pagePredict[pos][a] > 0){ 
                            bit_array[a] = 1;
                        }
                    }
                }
            }

            // This is more simplified than LRU
            // All pages that are not needed within the next three jumps are swapped out to free space
            for(proc_count = 0; proc_count < 15; proc_count++){
                if(bit_array[proc_count] == 0){
                    pageout(proc,proc_count);
                }
            }

            // Swap in all pages that are likely to be used in the next three jumps, this reduces thrashing significantly
            for(proc_count = 0; proc_count < 15; proc_count++){
                if(bit_array[proc_count] == 1){
                    pagein(proc, proc_count);
                }
            }
     }
} 
