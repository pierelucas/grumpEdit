/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef EDITOR_H_
#define EDITOR_H_

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "errorHandler.h"
#include "appendBuffer.h"

/* Global editor config. */
typedef struct _editorConfig_
{
    int screenrows;
    int screencols;
    struct termios orig_termios;
    struct termios raw_termios;
} editorconf;

/* Function for enabling the raw terminal. */
void enableRawMode(editorconf*);

/* Function for disabling the raw terminal. */
void disableRawMode(editorconf*);

/* Read Key and thow a error when read fails. */
char editorReadKey();

/* Process keypresses e.g keys and control keys. */
int editorProcessKeypress();

/* Draw a tilde (~) symbol at the beginning of each line. */
void editorDrawRows(editorconf*);

/* Clearing the screen. */
void editorRefreshScreen(editorconf*);

/* Get the actual cursor position. */
int editorGetCursorPosition(int*, int*);

/* Get the actual terminal window size with ioctl which calls TIICGWINSZ. */
int editorGetWindowSize(int*, int*);

#endif /* EDITOR_H_ */

