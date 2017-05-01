#ifndef SAFE_H
#define SAFE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <string.h> // String lib from C

#include <pthread.h>

// Unix specific headers
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 	// Includes a number of definitions of structures needed for sockets.
#include <netinet/in.h> 	// Constants and structures needed for internet domain addresses.
#include <netdb.h>				// Defines the structure 'hostent' wich is used in the client side.

pthread_mutex_t mutex;

FILE *safe_open(const char *name, const char *mode);

void *check_non_null(void *ptr);
void *safe_malloc(size_t size);
void *safe_calloc(size_t num, size_t size);
void *safe_realloc(void *ptr, size_t size);

#endif
