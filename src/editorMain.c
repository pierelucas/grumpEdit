/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "errorHandler.h"
#include "editor.h"

/* Global struct definition. */
static editorconf ee;
static editorconf* const eConfPtr = &ee;

/* Enable and disable terminal raw mode and do some cleaning work.  */
static void enableDisableRawMode(int state)
{
    if ( state == 0 )
    {
        enableRawMode(eConfPtr);
    }
    else
    {
        disableRawMode(eConfPtr);
        exit(0);
    }
}

/* Init editor. */
static void initEditor()
{
    if ( editorGetWindowSize(&(*eConfPtr).screenrows, &(*eConfPtr).screencols) == -1 )
    {
        errorHandler("editorGetWindowSize");
    }
}

/* Main function. */
int main(void)
{
    /* Allocate HEAP memory for editor config struct. */
    /* struct editorConfig* eConfPtr = (struct editorConfig*) malloc(sizeof(struct editorConfig)); */

    /* State 1 enables raw mode. */
    enableDisableRawMode(0);
    
    /* Call init editor function. */
    initEditor();
    
    while ( 1 )
    {
        if ( editorProcessKeypress() == -1 ) break; 
        editorRefreshScreen(eConfPtr);
    }
    
    /* Stat 2 disables raw mode and exit. */
    enableDisableRawMode(-1);
}

