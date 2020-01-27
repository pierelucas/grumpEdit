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
#include <stdbool.h>
#include <sys/ioctl.h>

#include "data.h"
#include "errorHandler.h"

/* Read Key and thow a error when read fails. */
char editorReadKey();

/* Process keypresses e.g keys and control keys. */
bool editorProcessKeypress();

/* Draw a tilde (~) symbol at the beginning of each line. */
void editorDrawRows();

/* Clearing the screen. */
void editorRefreshScreen();

/* Get the actual terminal window size with ioctl which calls TIICGWINSZ. */
bool editorGetWindowSize(int*, int*);

#endif /* EDITOR_H_ */

