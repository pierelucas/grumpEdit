/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "editor.h"

/*
 * Macro that bitwise AND the given integer on the same way that a CTRL Keypress doe.
*/
#define CTRL_KEY(T) ((T) & 0x1f)

/* __________________________________________________________________________*/
void disableRawMode(editorconf* eConfPtr)
{
    /*
     * Set the terminal to the original settings, saved in enbaleRawMode().
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &(*eConfPtr).orig_termios);
    if ( tcsetattr(STDIN_FILENO, TCSAFLUSH, &(*eConfPtr).orig_termios) == -1 )
    {
        errorHandler("tcsetattr");
    }
}

/* __________________________________________________________________________*/
void enableRawMode(editorconf* eConfPtr)
{
    /*
     * Copies the actual terminal settings to the orig_termios struct
     * and check throw error if that fails. In example a NULL pointer is given.
    */
    if ( tcgetattr(STDIN_FILENO, &(*eConfPtr).orig_termios) == -1 )
    {
        errorHandler("tcgetattr");
    }
    
    /*
     * Copies the actual terminal settings to the raw struct.
    */
    (*eConfPtr).raw_termios = (*eConfPtr).orig_termios;
    
    /*
     * BRKINT is turned on a break condition will cause a SIGINT signal to be sent to the program, like CTRL-C does.
     * INPCK enables a parity checking, that's maybe not work on modern terminal emulators.
     * ISTRIP causes the 8th bit of each input to be stripped (means that it's set to 0). On default, this is disabled on most systems.
     * ICRNL turns of the translation from CTRL-M '13' in '10' which is the ENTER key \n (newlineConfPtr).
     * IXON turns of the software flow control CTRL-S and CTRL-Q.
    */
    (*eConfPtr).raw_termios.c_iflag &= ~(BRKINT | INPCK | ISTRIP | /*ICRNL | */IXON);

    /*
     * OPOST turns out the output translation of '\n’ to '\r\n'.
     * That mean's that the newline just move the cursor down and not move the cursor down and on the beginning of the line.
    */
    (*eConfPtr).raw_termios.c_oflag &= ~(OPOST);

    /*
     * CS8 set's the character size to 8 bits per byte. This is the default on most systems. We set it just for provence.
    */
    (*eConfPtr).raw_termios.c_cflag &= ~(CS8);

    /*
     * ECHO turns outch echoing the output on the screen.
     * ICANON rad the input byte per byte instead line by line.
     * IEXTEN turns of CTRL-V which let the terminal waits for you type another character and then sends thats character literally.
     * ISIG turns out the CTRL-C and CTRL-Z signals which background and frozen the process
    */
    (*eConfPtr).raw_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /*
     * This set a timeout for the function read(). So hat read don't have to wair for an input to return.
     * VMIN are the max bytes needed for read() to returns.
     * VTIME are the amount of time to wait before read() returns in tenth of a second.
    */
    (*eConfPtr).raw_termios.c_cc[VMIN] = 0;       /* 0 byte needed. */
    (*eConfPtr).raw_termios.c_cc[VTIME] = 1;      /* 1/10 of a second (100 mseconds). */ 



    /*
     * Set the terminal to the modified settings
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &(*eConfPtr).raw_termios);
}

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
void editorDrawRows(editorconf* eConfPtr)
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
void editorRefreshScreen(editorconf* eConfPtr)
{
    /* 
     * Escape sequence (1b) + [ + Argument + J. 4 Bytes.
     * \x1b[2J clear the entire screen.
     * The 4 means we are writing 4 bytes to the terminal.
     */
    write(STDOUT_FILENO, "\x1b[2J", 4);

    /* This sequence reposition the cursor on the top. */
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows(eConfPtr);
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
    /*
     * winsize struct on function stack memory which can save the winsize called
     * by the TIOCGWINSZ call over ioctl.
    */
    struct winsize wSize;

    /* Use ioctl to get the window size via TIOCGWINSZ call. */
    if ( ioctl(STDOUT_FILENO, TIOCGWINSZ, &wSize) == 1 || wSize.ws_col == 0 )
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
        *cols = wSize.ws_col; 
        *rows = wSize.ws_row;
        return 0;
    }
}

