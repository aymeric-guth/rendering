#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <math.h>
//#include <ncurses.h>
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "fifo.h"
#include "constants.h"

void *kb_input(void *arg)
{
    Q *q = (Q *) arg;

    while (1) {
        int c = getchar();
        Q_put(q, c);
    }

    return NULL;
}


int main()
{
    pthread_t _kb_input;
    Q q = {0, 0, QUEUE_SIZE, malloc(sizeof(void *) * QUEUE_SIZE)};
    struct termios mode;
    tcgetattr(0, &mode);
    mode.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &mode);
    pthread_create(&_kb_input, NULL, kb_input, &q);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    printf("lines %d\n", w.ws_row);
    printf("columns %d\n", w.ws_col);
    return 0;  // make sure your main returns int

    while (1) {
        int elmt = 0;

        if (!Q_get(&q, &elmt))
            printf("Got elmt=%d\n", elmt);
    }

    return 0;
}
