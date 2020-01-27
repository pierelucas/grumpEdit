/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "errorHandler.h"

/* __________________________________________________________________________*/
void errorHandler(const char* s)
{
    /* Clean the terminal and reposition the cursor. */
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    
    /* Print out the error with prefix 's'. */ 
    perror(s);
    exit(1);
}

