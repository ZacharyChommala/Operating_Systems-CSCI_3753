/*
 * File: multi-lookup.c
 * Author: Derek Gorthy
 * SID: 102359021
 * Email: derek.gorthy@colorado.edu
 * Project: CSCI 3753 Programming Assignment 3
 * Description:
 * 	This file contains a solution for a multi-threaded DNS lookup-program.
 *  
 */

#include "multi-lookup.h"

// Declare queue, semaphores and global int
queue _q;
int _num_closed;

sem_t _queue;
sem_t _requesterThreads;


void* requester(void* myFile){

	// Handle filename
	char* file_name = (char*) myFile;
	FILE *file = NULL;
	int domains_pushed = 0;

	// Open file as read-only
	file = fopen(file_name, "r");
	if(!file){ // Check for error
		fprintf(stderr, "File couldn't be opened: %s\n", file_name);
		sem_wait(&_requesterThreads);
		_num_closed--;					// Tracks number of input files remaining, protected by semaphore
		sem_post(&_requesterThreads);
		return NULL;
	}

	// If file is opened properly
	else{
		int flag = 1; // Flag for while loop

		while(flag){
			char* hostname = malloc(MAX_NAME_LENGTH); // Dynamically allocate memory for current hostname being processed

			if(fscanf(file, INPUTFS, hostname) > 0){ // While items still left in input file
				int pushed = 0; // Variable to track if current hostname has been pushed

				while(pushed != 1){ // Loop until hostname is pushed
					sem_wait(&_queue); 
					if (!queue_is_full(&_q)){ // Only accessed if queue is not full
						if(queue_push(&_q, hostname) == QUEUE_FAILURE){
							fprintf(stderr, "Error: Queue failed on %s\n",hostname);
							free(hostname); // Free allocated memory							
						}
						else{
						domains_pushed++;
						pushed = 1; // Set value to exit loop and acquire new hostname
						}
						sem_post(&_queue);
					}
					else{ // Account for queue being full
						sem_post(&_queue);
						int random_time = rand()%100;
						usleep(random_time); // Sleep for a random amount of time
					}
				}
			}

			else{
				flag = 0;
				free(hostname); // Free allocated memory if no hostname
			}
		}

		fclose(file); // Close input file
		printf("Requester thread pushed %d hostnames to queue.\n", domains_pushed);

		sem_wait(&_requesterThreads);
		_num_closed--;
		sem_post(&_requesterThreads);
	}
	return 0;
}


void* resolver(void* myFile){

	// Handle filename
	char* file_name = (char*) myFile;
	FILE *file = NULL;

	int domains_popped = 0;
	char firstipstr[MAX_IP_LENGTH];
	char* hostname;
	int loop = 1; // Flag for the while loop

	// Open file to append only
	file = fopen(file_name, "a");
	if(!file){ // Check for error
		fprintf(stderr, "Error Opening Output File: %s\n", file_name);
		return NULL;
	}
	
    while(loop){
    	sem_wait(&_queue);

    	// Check if queue has domain names to be resolved
    	if(!queue_is_empty(&_q)){
			hostname = queue_pop(&_q);
			domains_popped++;

			// Resolve hostname and output error if not correct
		    if(dnslookup(hostname, firstipstr, sizeof(firstipstr))== UTIL_FAILURE){
				fprintf(stderr, "dnslookup error: %s\n", hostname); // Error message
				strncpy(firstipstr, "", sizeof(firstipstr));
		    }

		    // Write output to file
		    fprintf(file, "%s,%s\n", hostname, firstipstr);
		    free(hostname); // Release hostname memory
		}

		else{
			sem_wait(&_requesterThreads);
			if (_num_closed == 0){ // If all input files have been closed
				loop = 0; // Exit the loop
			}
			sem_post(&_requesterThreads);
		}
		sem_post(&_queue);
    }

    fclose(file); // Close output file
    printf("Resolver thread has processed %d hostnames.\n", domains_popped);
	return NULL;
}



int main(int argc, char* argv[]){
    
    // Check to make sure user provided the minimum number of arguments
    if(argc < MINARGS){
		fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
		fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
    }

    // Open file to write, this clears the output file
	FILE* file = NULL;
	file = fopen(argv[argc-1], "w");
	fclose(file);

	// Initialize semaphores
	sem_init(&_queue, 0, 1);
	sem_init(&_requesterThreads, 0, 1);

	// Initialize the queue
    const int qSize = TEST_SIZE;
    if(queue_init(&_q, qSize) == QUEUE_FAILURE){
		fprintf(stderr,
		"error: queue_init failed!\n");
    }

    int numResolvers = MIN_RESOLVER_THREADS; // Set the number of resolvers 2
	int numCores = sysconf(_SC_NPROCESSORS_ONLN); // The number of cores in the CPU
	if (numCores <= MAX_RESOLVER_THREADS && numCores >= MIN_RESOLVER_THREADS){
		numResolvers = numCores; // Set number of resolver threads to number of cores
	}

    int num_input_files = argc-2;
 	pthread_t file_threads[num_input_files];
 	pthread_t resolver_threads[numResolvers];
 	int rc;
 	_num_closed = num_input_files;

 	// Create a requester thread for every input file
 	// Assign it to the file
    for(int t=0;t<num_input_files;t++){
		rc = pthread_create(&(file_threads[t]), NULL, requester, (void*) argv[t+1]);
		if (rc){
		    printf("ERROR; return code from pthread_create() is %d\n", rc);
		    exit(EXIT_FAILURE);
		}
    }

    // Create a resolver thread to coorespond to the number of cores
    for(int k=0;k<numResolvers;k++){
    	rc = pthread_create(&(resolver_threads[k]), NULL, resolver, (void*) argv[argc-1]);
    	if (rc){
    		printf("ERROR; return code from pthread_create() is %d\n", rc);
    		exit(EXIT_FAILURE);
    	}
    }

    // Wait for all requester threads to finish, join
    for(int a=0;a<num_input_files;a++){
		pthread_join(file_threads[a],NULL);
    }
    printf("All of the file threads were completed!\n");

    // Wait for all resolver threads to finish, join
    for(int b=0;b<numResolvers;b++){
		pthread_join(resolver_threads[b],NULL);
    }
    printf("All of the resolver threads were completed!\n");

    // Destroy semaphores and cleanup queue
   	sem_destroy(&_queue);
	sem_destroy(&_requesterThreads);
	queue_cleanup(&_q);

    return EXIT_SUCCESS;
}