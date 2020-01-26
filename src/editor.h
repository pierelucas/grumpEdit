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

#include "errorHandler.h"

/* Read Key and thow a error when read fails. */
char editorReadKey();

/* Process keypresses e.g keys and control keys. */
bool editorProcessKeypress();

/* Clearing the screen. */
void editorRefreshScreen();

#endif /* EDITOR_H_ */

