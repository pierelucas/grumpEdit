/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include <stdbool.h>

#include "terminal.h"
#include "errorHandler.h"
#include "editor.h"

/* Enable and disable terminal raw mode. */
static void enableDisableRawMode(bool state)
{
    if ( state )
    {
        enableRawMode();
    }
    else
    {
        disableRawMode();
        exit(0);
    }
}

/* Main function. */
int main(void)
{
    enableDisableRawMode(1);
    
    while ( 1 )
    {
        if ( !editorProcessKeypress() ) { break; }
        editorRefreshScreen();
    }

    enableDisableRawMode(0);
}

