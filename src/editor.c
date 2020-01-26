/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "editor.h"

/*
 * Macro that bitwise and the given integer on the same way that a CTRL Keypress doe.
*/
#define CNTRL_KEY(T) ((T) & 0x1f)

/* __________________________________________________________________________*/
char editorReadKey()
{
    int nread;
    char c;
    while ( (nread = read(STDIN_FILENO, &c, 1)) != 1 )
    {
        if ( nread == -1 && errno != EAGAIN )
        {
            errorHandler("read");
        }
    }
    return c;
}

/* __________________________________________________________________________*/
bool editorProcessKeypress()
{
    char c = editorReadKey();

    switch (c)
    {
     case CNTRL_KEY('q'):
         return 0;
    }
    return 1;
}

/* __________________________________________________________________________*/
void editorRefreshScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

