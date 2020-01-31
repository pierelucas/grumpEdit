/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include <getopt.h>

#include "errorHandler.h"
#include "editor.h"

/* Struct definition. */
static editorconf ee;
static editorconf* const eConfPtr = &ee;

/* Filename */
static char* kfilename;

/* Parse command line arguments- */
static char* parseCommandLineArguments(int argc, char** argv)
{
    /*
    if ( argc == 1 )
    {
        fprintf(stdout, "One argument needed.\n");
        fprintf(stdout, "-h         -- for help message.\n");
        exit(1);
    }
    */

    int opt = getopt(argc, argv, ":hf:");
    while ( opt != -1 )
    {
        switch (opt)
        {
         case 'h':
             fprintf(stdout, "HELP\n");
             fprintf(stdout, "Options:\n");
             fprintf(stdout, "%10s -f <filename>    -- specify file.\n", *(argv+0));
             fprintf(stdout, "%10s -h               -- for help message.\n", *(argv+0));
             exit(1);
        case 'f':
             return optarg;
        case ':':
             fprintf(stdout, "option: %c needs a value.\n", optopt);
             exit(1);
        case '?':
             fprintf(stdout, "unknown option: %c\n", optopt);
             exit(1);
        }
    }
    return NULL;
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
    kfilename = parseCommandLineArguments(argc, argv);
    
    /* Cast kfilename to constant pointer */
    kfilename = (char* const) kfilename;

    /* Allocate HEAP memory for editor config struct. */
    /* editorconf* eConfPtr = (editorconf*) malloc(sizeof(editorconf)); */

    /* State 1 enables raw mode. */
    enableRawMode(eConfPtr);
    
    /* Call init editor function. */
    initEditor();

    /* Open file. */
    editorOpen(eConfPtr, kfilename);
    
    do
    {
        editorRefreshScreen(eConfPtr);
    } while ( editorProcessKeypress(eConfPtr) != -1 );
    
    /* Disable raw mode & reset terminal. */
    disableRawMode(eConfPtr);

    /* Exit successfully. */
    return EXIT_SUCCESS;
}

