/*
 * File: multi-lookup.h
 * Author: Derek Gorthy
 * SID: 102359021
 * Email: derek.gorthy@colorado.edu
 * Project: CSCI 3753 Programming Assignment 3
 * Description:
 * 	This file is a header file for multi-lookup.c
 *  
 */

#ifndef MULTILOOKUP_H
#define MULTILOOKUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include "util.h"
#include "queue.h"

#define MINARGS 3
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"
#define TEST_SIZE 10
#define MAX_NAME_LENGTH 1025
#define MAX_RESOLVER_THREADS 20
#define MIN_RESOLVER_THREADS 2
#define MAX_IP_LENGTH INET6_ADDRSTRLEN // This number is 46

void* requester(void* filename);
void* resolver(void* output);

#endif