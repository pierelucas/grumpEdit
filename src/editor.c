/*
 * Copyright 2020 (C) Julian Huch
 * Autor: Julian Huch
*/

#include "editor.h"
#include "version.h"    /* This header holds some const variables. */

/* __________________________________________________________________________*/
void disableRawMode(editorconf* const eConfPtr)
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
void enableRawMode(editorconf* const eConfPtr)
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
int editorReadKey()
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
    
    /* Check if 'c' is an escape sequence. */
    if ( c == '\x1b' )
    {
        char sequence[3];
        
        /* 
         * Read the next two characters from bufffer in our sequence variable.
         * Return an escape when read timeout (means buffer is empty).
        */
        if ( read(STDIN_FILENO, &(*(sequence+0)), 1) != 1 ) return '\x1b';
        if ( read(STDIN_FILENO, &(*(sequence+1)), 1) != 1 ) return '\x1b';
        
        /*
         * When the next byte is a [ start to read the escape sequence arguments. 
        */
        if ( *(sequence+0) == '[')
        {
            if ( *(sequence+1) >= '0' && *(sequence+1) <= '9')
            {
                /*
                 * Read the next character from buffer in our sequence variable.
                 * Return an escape when read timeout (means buffer is empty).
                */
                if ( read(STDIN_FILENO, &(*(sequence+2)), 1) != 1 ) return '\x1b';
                /*
                 * Check if the last character is an '~'.
                 * The escape sequence from the keys:
                 * '\x1b[5~' PAGE_UP
                 * '\x1b[6~' PAGE_DOWN
                 * '\x1b[1~' HOME_KEY
                 * '\x1b[7~' HOME_KEY
                 * '\x1b[4~' END_KEY
                 * '\x1b[8~' END_KEY
                */
                if ( *(sequence+2) == '~' )
                {
                    switch ( *(sequence+1) )
                    {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            }
            /*
             * The escape sequence from the keys:
             * '\x1b[A' ARROW_UP
             * '\x1b[B' ARROW_DOWN
             * '\x1b[C' ARROW_RIGHT
             * '\x1b[D' ARROW_LEFT
             * '\x1b[H' HOME_KEY
             * '\x1b[F' END_KEY
            */
            switch ( *(sequence+1) )
            {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
        /* When the next byte is 'O' ... */
        else if ( *(sequence+0) == 'O' )
        {
            /*
             * The escape sequence from the keys:
             * '\x1bOH' HOME_KEY
             * '\x1bOF' END_KEY
            */
            switch ( *(sequence+1) )
            {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            } 
        }
        /* When the next byte isn't a '[’ return escape. */
        return '\x1b';
    }
    /* When c is not a escape sequence, return c. */
    else
    {
        return c;
    }
}

/* __________________________________________________________________________*/
void editorMoveCursor(editorconf* const eConfPtr, int key)
{
    /*
     * Don't let the cursor be out of window. The documentation say that's
     * undefined what happens, when the cursor get out of the window.
     * Increment or decrement the CursorX and CursorY value.
    */
    switch ( key )
    {
        case ARROW_LEFT:
            if ( (*eConfPtr).cursorX != 0 ) (*eConfPtr).cursorX--;
            break;
        case ARROW_RIGHT:
            if ( (*eConfPtr).cursorX != (*eConfPtr).screencols -1 ) (*eConfPtr).cursorX++;
            break;
        case ARROW_UP:
            if ( (*eConfPtr).cursorY != 0 ) (*eConfPtr).cursorY--;
            break;
        case ARROW_DOWN:
            if ( (*eConfPtr).cursorY < (*eConfPtr).numrows ) (*eConfPtr).cursorY++;
            break;
    }
}

/* __________________________________________________________________________*/
int editorProcessKeypress(editorconf* const eConfPtr)
{
    int c = editorReadKey();

    switch (c)
    {
        case QUIT_EDITOR:
            return -1;
        
        case HOME_KEY:
           (*eConfPtr).cursorX = 0;
           break;
        case END_KEY:
           (*eConfPtr).cursorX = (*eConfPtr).screencols - 1;
           break;
        
        /* Set the cursor to the top and end when PAGE_UP or PAGE_DOWN is pressed. */ 
        case PAGE_UP:
        case PAGE_DOWN:
            {
                int times = (*eConfPtr).screenrows;
                while ( times-- )
                {
                    editorMoveCursor(eConfPtr, c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
                }
                break;
            }
        
        /* When a ARROW_KEY is pressed call 'editorMoveCursor'. */
        case ARROW_LEFT:
        case ARROW_RIGHT:
        case ARROW_UP:
        case ARROW_DOWN:
            editorMoveCursor(eConfPtr, c); 
            break;
    }
    return 0;
}

/* __________________________________________________________________________*/
void editorScroll(editorconf* const eConfPtr)
{
    if ( (*eConfPtr).cursorY < (*eConfPtr).rowoff )
    {
        (*eConfPtr).rowoff = (*eConfPtr).cursorY;
    }

    if ( (*eConfPtr).cursorY >= (*eConfPtr).rowoff + (*eConfPtr).screenrows )
    {
        (*eConfPtr).rowoff = (*eConfPtr).cursorY - (*eConfPtr).screenrows + 1;
    }
}

/* __________________________________________________________________________*/
void editorDrawWelcomeMessage(editorconf* const eConfPtr, appendbuffer* const aBufPtr)
{
    /* Define 80xsizeof(char) array for welcome message. */
    char welcome[80];
    
    /*
     * Count the formated string (same as printf does) as C String in a array.
     * Return the number of characters that would be written.
    */
    int welcomeLen = snprintf(welcome, sizeof(welcome),
                              "Grump Edit -- VERSION: %s", GRUMPEDIT_V);

    if ( welcomeLen > (*eConfPtr).screencols )
    {
        welcomeLen = (*eConfPtr).screencols;
    }

    int padding = ((*eConfPtr).screencols - welcomeLen) / 2;
    if ( padding )
    {
        aBufferAppend(aBufPtr, "~", 1);
        padding--;
    }

    while ( padding-- ) aBufferAppend(aBufPtr, " ", 1);
    
    /* Append the welcome message to buffer. */
    aBufferAppend(aBufPtr, welcome, welcomeLen);
}

/* __________________________________________________________________________*/
void editorDrawRows(editorconf* const eConfPtr, appendbuffer* const aBufPtr)
{
    int i;
    for (i = 0; i < (*eConfPtr).screenrows; ++i)
    {
        int filerow = i + (*eConfPtr).rowoff;

        /* 
         * Call 'editorDrawWelcomeMessage' at middle of the window height
         * when there's no data readed.
        */
        if ( filerow >= (*eConfPtr).numrows )
        {
            if ( (*eConfPtr).numrows == 0 && i == (*eConfPtr).screenrows / 3 )
            {
                editorDrawWelcomeMessage(eConfPtr, aBufPtr);
            }
            else
            {
                aBufferAppend(aBufPtr, "~", 1);
            }        
        }
        /* When the datastructure is not empty write the rows to the buffer. */
        else
        {
            int len = (*(((*eConfPtr).row)+filerow)).size;    /* Is the same as 'eConfPtr->row[i]->size' */
            if ( len > (*eConfPtr).screencols ) len = (*eConfPtr).screencols;
            aBufferAppend(aBufPtr, (*(((*eConfPtr).row)+filerow)).chars, len);    /* Is the same as 'eConfPtr->row[i]->chars' */
        }

        /* 
         * Clear the line right of the cursor 0K default argument).
         * 2K Erases the whole line.
         * 1K Erased the line left of the cursor.
        */
        aBufferAppend(aBufPtr, "\x1b[K", 3);
        
        /*
         * Append '\r\n' to all lines, except the last line.
         * That's important to didn't had a blank line, without tilde,
         * at the bottom of the window.
        */
        if ( i < (*eConfPtr).screenrows - 1 )
        {
            aBufferAppend(aBufPtr, "\r\n", 2);
        }
    }
}

/* __________________________________________________________________________*/
void editorRefreshScreen(editorconf* const eConfPtr)
{
    editorScroll(eConfPtr);

    /* Initialize buffer. */
    appendbuffer aBuf = { NULL, 0 };
    appendbuffer* const aBufPtr = &aBuf;
    
    /* Hide Cursor before refreshing the screen. */
    aBufferAppend(aBufPtr, "\x1b[?25l", 6);

    /* 
     * Escape sequence (1b) + [ + Argument + J. 4 Bytes.
     * \x1b[2J clear the entire screen.
     * The 4 means we are writing 4 bytes to the terminal.
    aBufferAppend(aBufPtr, "\x1b[2J", 4);
    */

    /* This sequence reposition the cursor on the top. */
    aBufferAppend(aBufPtr, "\x1b[H", 3);
    
    /* Call editorDrawRows. */
    editorDrawRows(eConfPtr, aBufPtr);
    
    /* Write the cursor position to buffer. */
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (*eConfPtr).cursorY + 1,
             (*eConfPtr).cursorX + 1);
    aBufferAppend(aBufPtr, buf, strlen(buf));
    
    /* Display the cursor after refreshing the screen. */
    aBufferAppend(aBufPtr, "\x1b[?25h", 6);
    
    /* Write out the buffer to STDOUT. */
    write(STDOUT_FILENO, (*aBufPtr).b, (*aBufPtr).len);

    /* Free memory. */
    aBufferFree(aBufPtr);
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

/* __________________________________________________________________________*/
void editorAppendRow(editorconf* const eConfPtr, char* s, size_t len)
{
    /* Re-allocate memory to add a new editorrow at the end of the editorrow array. */
    (*eConfPtr).row = (editorrow*) realloc((*eConfPtr).row, sizeof(editorrow) * ((*eConfPtr).numrows + 1));

    (*(*eConfPtr).row).size = len;      /* Is the same as 'eContPtr->row->size' */
    (*(*eConfPtr).row).chars = (char*) malloc(len+1);
    memcpy((*(*eConfPtr).row).chars, s, len);
    *((*(*eConfPtr).row).chars+len) = '\0';     /* Is the same as 'eConftPtr->row->chars[len]' */
    (*eConfPtr).numrows++;
}

/* __________________________________________________________________________*/
void editorOpen(editorconf* const eConfPtr, char* const filename)
{
    /*
    char* line = "Hello, World!";
    ssize_t linelen = 13;

    (*eConfPtr).row.size = linelen;
    (*eConfPtr).row.chars = (char*) malloc(linelen + 1);
    memcpy((*eConfPtr).row.chars, line, linelen);
    *((*eConfPtr).row.chars + linelen) = '\0';
    (*eConfPtr).numrows = 1;
    */

    /* Return when the filename is NULL. */
    if ( filename == NULL ) return;

    /* Declare FILE pointer and open file. */
    FILE* fp;
    fp = fopen(filename, "r");

    /* When it's not possible to allcate for file then throw a error. */
    if ( fp == NULL ) errorHandler("fopen");

    char* line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    /* Read every line till end of file is reached. */
    while ( (linelen = getline(&line, &linecap, fp)) != -1 )
    {
        for ( ; linelen > 0 && ( *(line+(linelen-1)) == '\n' ||
                                 *(line+(linelen-1)) == '\r');
                                 linelen--);

        /* Same as:
        while ( linelen > 0 &&  ( *(line+(linelen-1)) == '\n' ||
                                  *(line+(linelen-1)) == '\r'))
        {
            linelen--;
        }
        */

        /* Call editorAppendRow. */
        editorAppendRow(eConfPtr, line, linelen);    
    } 

    /* Free allocated memory. */
    free(line);

    /* Close file. */
    fclose(fp);
}

