/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include <stdbool.h>

#include "terminal.h"
#include "errorHandler.h"
#include "editor.h"
#include "data.h"

/* Static pointers to our data structures. */
static struct editorConfig* const eConfPtr = &eConf;

/* Enable and disable terminal raw mode and do some cleaning work.  */
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

/* Init editor. */
static void initEditor()
{
    if ( !(editorGetWindowSize(&(*eConfPtr).screenrows, &(*eConfPtr).screencols)) )
    {
        errorHandler("editorGetWindowSize");
    }
}

/* Main function. */
int main(void)
{
    /* State 1 enables raw mode. */
    enableDisableRawMode(1);
    
    /* Call init editor function. */
    initEditor();
    
    while ( 1 )
    {
        if ( !editorProcessKeypress() ) { break; }
        editorRefreshScreen();
    }
    
    /* Stat 2 disables raw mode and exit. */
    enableDisableRawMode(0);
}

