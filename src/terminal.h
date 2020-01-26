/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <termios.h>
#include <unistd.h>

#include "errorHandler.h"

/* Data structure for saving terminal settings. */
extern struct termios orig_termios;
extern struct termios raw;

/* Function for enabling the raw terminal. */
void enableRawMode();

/* Function for disabling the raw terminal. */
void disableRawMode();

#endif /* TERMINAL_H_ */

