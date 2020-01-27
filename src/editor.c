/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "editor.h"

/*
 * Macro that bitwise AND the given integer on the same way that a CTRL Keypress doe.
*/
#define CNTRL_KEY(T) ((T) & 0x1f)

/* Static pointers to our data structures. */
static struct winsize* const kWSizePtr = &wsize;
static struct editorConfig* const eConfPtr = &eConf;

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
void editorDrawRows()
{
    for (int i = 0; i < (*eConfPtr).screenrows; ++i)
    {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

/* __________________________________________________________________________*/
void editorRefreshScreen()
{
    /* 
     * Escape sequence (1b) + [ + Argument + J. 4 Bytes.
     * \x1b[2J clear the entire screen.
     * The 4 means we are writing 4 bytes to the terminal.
     */
    write(STDOUT_FILENO, "\x1b[2J", 4);

    /* This sequence reposition the cursor on the top. */
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/* __________________________________________________________________________*/
bool editorGetWindowSize(int* rows, int* cols)
{
    if ( ioctl(STDOUT_FILENO, TIOCGWINSZ, kWSizePtr) == 1 || (*kWSizePtr).ws_col == 0 )
    {
         return 0;
    }
    else
    {
        *cols = (*kWSizePtr).ws_col; 
        *rows = (*kWSizePtr).ws_row;
        return 1;
    }
}

