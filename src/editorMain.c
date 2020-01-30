/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include <getopt.h>

#include "errorHandler.h"
#include "editor.h"

/* Global struct definition. */
static editorconf ee;
static editorconf* const eConfPtr = &ee;

static char* parseCommandLineArguments(int argc, char** argv)
{
    int opt = getopt(argc, argv, ":f:");

    while ( opt != -1 )
    {
        // TODO
        return "peter";
    }
    return "peter";
}

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
    /* Initialize the cursor position. */
    (*eConfPtr).cursorX = 0;    /* X = Column. */
    (*eConfPtr).cursorY = 0;    /* Y = Row. */
    (*eConfPtr).numrows = 0;

    if ( editorGetWindowSize(&(*eConfPtr).screenrows, &(*eConfPtr).screencols) == -1 )
    {
        errorHandler("editorGetWindowSize");
    }
}

/* Main function. */
int main(int argc, char** argv)
{
    char* filename;
    if ( argc != 0 )
    {
        filename = parseCommandLineArguments(argc, argv);
    }
    else
    {
        filename = NULL;
    }
    
    /* Cast filename to constant. */
    filename = (char* const) filename;

    /* Allocate HEAP memory for editor config struct. */
    /* editorconf* eConfPtr = (editorconf*) malloc(sizeof(editorconf)); */

    /* State 1 enables raw mode. */
    enableDisableRawMode(0);
    
    /* Call init editor function. */
    initEditor();

    /* Open file. */
    editorOpen(eConfPtr, filename);
    
    do
    {
        editorRefreshScreen(eConfPtr);
    } while ( editorProcessKeypress(eConfPtr) != -1 );
    
    /* Stat 2 disables raw mode and exit. */
    enableDisableRawMode(-1);
}

