/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include <stdlib.h>
#include <stdio.h>

/* 
 * Function that handle's occuring errors and print out the errno message.
 * The given string pointer is printed before the errno message.
 * Program exit with exit code 1
*/
void errorHandler(const char*);

#endif /* ERRORHANDLER_H_ */

