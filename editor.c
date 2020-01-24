#include "editor.h"

// ____________________________________________________________________________
void enableRawMode()
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO);
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
