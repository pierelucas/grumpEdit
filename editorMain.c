#include "editor.h"

int main(void)
{
    enableRawMode();

    char c;
    while ( read(STDIN_FILENO, &c, 1)  == 1 && c != 'q' );
    return EXIT_SUCCESS;
}
