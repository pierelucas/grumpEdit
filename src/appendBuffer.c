/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "appendBuffer.h"

/* __________________________________________________________________________*/
void aBufferAppend(appendbuffer* const ab, const char* s, int len)
{
    char* new = realloc((*ab).b, (*ab).len + len);

    if ( new == NULL ) return;
    memcpy((new+(*ab).len), s, len);
    (*ab).b = new;
    (*ab).len += len; 
}

/* __________________________________________________________________________*/
void aBufferFree(appendbuffer* const ab)
{
    free((*ab).b);
}

