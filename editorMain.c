/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "editor.h"

/* exit Functions that's do same garbage work and exit the programm */
static void editorExit()
{
    disableRawMode();
    exit(0);
}

/* Main function. */
int main(void)
{
    enableRawMode();
    
    readInput();

    editorExit();
}

