/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef EDITOR_H_
#define EDITOR_H_

/*
 * Macro that bitwise AND the given integer on the same way that a CTRL Keypress doe.
*/
#define CTRL_KEY(T) ((T) & 0x1f)

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>

#include "errorHandler.h"
#include "appendBuffer.h"

/* Data structure for storing editor rows. */
typedef struct _editorRow_
{
    int size;
    char* chars;
} editorrow;

/* Global editor config. */
typedef struct _editorConfig_
{
    int cursorX, cursorY;
    int screenrows, screencols;
    int numrows;
    editorrow row;
    struct termios orig_termios;
    struct termios raw_termios;
} editorconf;

/* Constant for Key sqeuences. */
enum editorKeys
{
    QUIT_EDITOR = CTRL_KEY('q'),
    ARROW_LEFT = CTRL_KEY('h'),   /* 1000 */
    ARROW_RIGHT = CTRL_KEY('l'),  /* 1001 */
    ARROW_UP = CTRL_KEY('k'),     /* 1002 */
    ARROW_DOWN = CTRL_KEY('j'),    /* 1003 */
    DEL_KEY = 1004,
    HOME_KEY = 1005,
    END_KEY = 1006,
    PAGE_UP = 1007,
    PAGE_DOWN = 1008
};

/* Function for enabling the raw terminal. */
void enableRawMode(editorconf* const);

/* Function for disabling the raw terminal. */
void disableRawMode(editorconf* const);

/* Read Key and thow a error when read fails. */
int editorReadKey();

/* Move the cursor around. */
void editorMoveCursor(editorconf* const, int);

/* Process keypresses e.g keys and control keys. */
int editorProcessKeypress(editorconf* const);

/* Draw a fancy welcome message. */
void editorDrawWelcomeMessage(editorconf* const, appendbuffer* const);

/* Draw a tilde (~) symbol at the beginning of each line. */
void editorDrawRows(editorconf* const, appendbuffer* const);

/* Clearing the screen. */
void editorRefreshScreen(editorconf* const);

/* Get the actual cursor position. */
int editorGetCursorPosition(int*, int*);

/* Get the actual terminal window size with ioctl which calls TIICGWINSZ. */
int editorGetWindowSize(int*, int*);

/* Open a file. */
void editorOpen(editorconf* const);

#endif /* EDITOR_H_ */

