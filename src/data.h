/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef DATA_H_
#define DATA_H_

#include <termios.h>
#include <sys/ioctl.h>

/* Global editor config. */
struct editorConfig
{
    int screenrows;
    int screencols;
    struct termios orig_termios;
    struct termios raw_termios;
};

/* Data structures. */
struct editorConfig eConf;
struct winsize wsize;

#endif /* DATA_H_ */

