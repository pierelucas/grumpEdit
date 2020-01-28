/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef APPENDBUFFER_H_
#define APPENDBUFFER_H_

#include <stdlib.h>
#include <string.h>

/* Append Buffer. */
struct appendBuffer
{
    char* b;
    int len;
};

extern struct appendBuffer aBuf;

void aBufferAppend(struct appendBuffer*, const char*, int);

void aBufferFree(struct appendBuffer*);

#endif /* APPENDBUFFER_H_ */

