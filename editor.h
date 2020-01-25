/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

extern struct termios orig_termios;
extern struct termios raw;

/*
static struct termios orig_termios;
static struct termios* const orig_termiosPtr = &orig_termios;

static struct termios raw;
static struct termios* const rawPtr = &raw;
*/

void disableRawMode();

void enableRawMode();

void readInput();

