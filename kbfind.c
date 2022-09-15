#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <math.h>
#include <ncurses.h>
#include <pthread.h>

#include "fifo.h"
#include "constants.h"
#include <stdio.h>
#include <termios.h>

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

    while (1) {
        int elmt = 0;

        if (!Q_get(&q, &elmt))
            printf("Got elmt=%d\n", elmt);
    }

    return 0;
}
