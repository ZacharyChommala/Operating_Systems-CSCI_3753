// Author: Derek Gorthy
// SID: 102359021
// References: Jack Dinkel, Luke Meszar, Sean Harris
//
// Description: This solution determines which page was least recently used and swaps it out for the next
// page that is needed.

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
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;
    int pc;
    int page;
    int proc;
    int jmin;
    int cur_lru;

    /* initialize static vars on first run */
    if(!initialized){
        for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
            for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
                timestamps[proctmp][pagetmp] = 0;
            }
        }
        initialized = 1;
    }

    // Must loop through all processes to make sure that every single page that is in is checked
    for(proc=0; proc<MAXPROCESSES; proc++) {
        // Determine if the process is active
        if(q[proc].active) {
            pc = q[proc].pc;                // determine program counter
            page = pc/PAGESIZE;             // page the program counter needs
            // Update when page was last used
            timestamps[proc][page] = tick;
            if(!q[proc].pages[page]) {
                // Attempt to swap page in, will enter if statement if all page slots are filled
                if(!pagein(proc,page)) {
                    // Determine which page was used least recently
                    cur_lru = -1; // Initialize the current LRU to -1 (will always change)
                    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
                        // Will be true if page is in and the timestamp shows it was used least recently
                        if((tick - timestamps[proc][pagetmp]) > cur_lru && q[proc].pages[pagetmp]) {
                            // Save the temp page
                            jmin = pagetmp;
                            cur_lru = (tick - timestamps[proc][pagetmp]);
                        }
                    }
                    pageout(proc,jmin);
                }
            }
        }
        tick++;
    }
} 
