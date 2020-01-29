/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef APPENDBUFFER_H_
#define APPENDBUFFER_H_

#include <stdlib.h>
#include <string.h>

/*
 * Append buffer.
 * Please initialize the struct with {NULL, 0} to work properly.
*/
typedef struct _appendBuffer_
{
    char* b;
    int len;
} appendbuffer;

/* Allocate new memory for s, append and copy in the appendbuffer. */
void aBufferAppend(appendbuffer* const, const char*, int);

/* Free allocated appendbuffer memory. */
void aBufferFree(appendbuffer* const);

#endif /* APPENDBUFFER_H_ */

