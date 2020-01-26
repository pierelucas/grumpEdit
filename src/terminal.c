/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "terminal.h"

/* Struct and pointer definition for the original terminal settings. */
struct termios orig_termios;
struct termios* const orig_termiosPtr = &orig_termios;

/* Struct and pointer definition for the raw terminal settings. */
struct termios raw;
struct termios* const rawPtr = &raw;

/* __________________________________________________________________________*/
void disableRawMode()
{
    /*
     * Set the terminal to the original settings, saved in enbaleRawMode().
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termiosPtr);
    if ( tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termiosPtr) == -1 )
    {
        errorHandler("tcsetattr");
    }
}

/* __________________________________________________________________________*/
void enableRawMode()
{
    /*
     * Copies the actual terminal settings to the orig_termios struct
     * and check throw error if that fails. In example a NULL pointer is given.
    */
    if ( tcgetattr(STDIN_FILENO, orig_termiosPtr) == -1 )
    {
        errorHandler("tcgetattr");
    }
    
    /*
     * Copies the actual terminal settings to the raw struct.
    */
    *rawPtr = *orig_termiosPtr;
    
    /*
     * BRKINT is turned on a break condition will cause a SIGINT signal to be sent to the program, like CTRL-C does.
     * INPCK enables a parity checking, that's maybe not work on modern terminal emulators.
     * ISTRIP causes the 8th bit of each input to be stripped (means that it's set to 0). On default, this is disabled on most systems.
     * ICRNL turns of the translation from CTRL-M '13' in '10' which is the ENTER key \n (newline).
     * IXON turns of the software flow control CTRL-S and CTRL-Q.
    */
    (*rawPtr).c_iflag &= ~(BRKINT | INPCK | ISTRIP | /*ICRNL | */IXON);

    /*
     * OPOST turns out the output translation of '\n’ to '\r\n'.
     * That mean's that the newline just move the cursor down and not move the cursor down and on the beginning of the line.
    */
    (*rawPtr).c_oflag &= ~(OPOST);

    /*
     * CS8 set's the character size to 8 bits per byte. This is the default on most systems. We set it just for provence.
    */
    (*rawPtr).c_cflag &= ~(CS8);

    /*
     * ECHO turns outch echoing the output on the screen.
     * ICANON rad the input byte per byte instead line by line.
     * IEXTEN turns of CTRL-V which let the terminal waits for you type another character and then sends thats character literally.
     * ISIG turns out the CTRL-C and CTRL-Z signals which background and frozen the process
    */
    (*rawPtr).c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /*
     * This set a timeout for the function read(). So hat read don't have to wair for an input to return.
     * VMIN are the max bytes needed for read() to returns.
     * VTIME are the amount of time to wait before read() returns in tenth of a second.
    */
    (*rawPtr).c_cc[VMIN] = 0;       /* 0 byte needed. */
    (*rawPtr).c_cc[VTIME] = 1;      /* 1/10 of a second (100 mseconds). */ 



    /*
     * Set the terminal to the modified settings
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, rawPtr);
}

