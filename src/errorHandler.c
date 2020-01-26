/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "errorHandler.h"

/* __________________________________________________________________________*/
void errorHandler(const char* s)
{
    perror(s);
    exit(1);
}

