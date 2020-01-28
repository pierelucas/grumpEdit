/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "editor.h"

/*
 * Macro that bitwise AND the given integer on the same way that a CTRL Keypress doe.
*/
#define CTRL_KEY(T) ((T) & 0x1f)

/* Static pointers to our data structures. */
static struct winsize* const kWSizePtr = &wsize;
static struct editorConfig* const eConfPtr = &eConf;
static struct appendBuffer* const aBufPtr = &aBuf;

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
int editorProcessKeypress()
{
    char c = editorReadKey();

    switch (c)
    {
     case CTRL_KEY('q'):
         return -1;
    }
    return 0;
}

/* __________________________________________________________________________*/
void editorDrawRows()
{
    int i;
    for (i = 0; i < (*eConfPtr).screenrows; ++i)
    {
        write(STDOUT_FILENO, "~", 1);
        
        /*
         * Append '\r\n' to all lines, except the last line.
         * That's important to didn't had a blank line, without tilde,
         * at the bottom of the window.
        */
        if ( i < (*eConfPtr).screenrows - 1 )
        {
            write(STDOUT_FILENO, "\r\n", 2);
        }
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
int editorGetCursorPosition(int* rows, int* cols)
{
    char buffer[32];
    unsigned int i = 0;
    
    /*
     * The n command can be used to query terminal status information
     * the argument 6 ask for the current cursor position.
     * We can read the return from the STDIN.
    */
    if ( write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    
    for ( ; i < sizeof(buffer) - 1; ++i )
    {
        if ( read(STDIN_FILENO, &(*(buffer+i)), 1) != 1 ) break;
        if ( *(buffer+i) == 'R' ) break;
    }
    
    /* We had to assign a '\0' line end to the last address of the buffer. */
    *(buffer+i) = '\0';
    
    /*
     * Test the 6n command.   
    printf("\r\n&buf[1]: '%s'\r\n", &buffer[1]);
    editorReadKey();
    */
    
    if ( *(buffer+0) != '\x1b' || *(buffer+1) != '[' ) return -1;
    if ( sscanf(&(*(buffer+2)), "%d;%d", rows, cols) != 2 ) return -1;

    return 0;
}

/* __________________________________________________________________________*/
int editorGetWindowSize(int* rows, int* cols)
{
    /* Use ioctl to get the window size via TIOCGWINSZ call. */
    if ( ioctl(STDOUT_FILENO, TIOCGWINSZ, kWSizePtr) == 1 || (*kWSizePtr).ws_col == 0 )
    {
        /*
         * ioctl don't work on all systems, so we had to implement a fallback.
         * 999C Moves the cursor to the right and 999B moves the cursor down.
        */
        if ( write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return editorGetCursorPosition(rows, cols); /* Returns -1 or 0. */
    }
    else
    {
        *cols = (*kWSizePtr).ws_col; 
        *rows = (*kWSizePtr).ws_row;
        return 0;
    }
}

