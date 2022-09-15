#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

#include "utilz.h"
// https://gist.github.com/ryankurte/61f95dc71133561ed055ff62b33585f8

struct MQ {
    int c;
    struct MQ *next;
};
typedef struct MQ MQ;

void MQ_enqueue(MQ *q, int c)
{
    printf("MQ_enqueue\n");
    MQ *last = NULL;

    if (!q) {
        q = (MQ *)malloc(sizeof(MQ));

        if (!q)
            exit(-1);

        last = q;
        q->next = NULL;
    }

    while (q->next != NULL) {
        last = q;
        q = q->next;
    }

    MQ *p = (MQ *)malloc(sizeof(MQ));

    if (!p)
        exit(-1);

    memset(p, 0, sizeof(MQ));
    p->c = c;
    p->next = NULL;
    last->next = p;
}

int MQ_dequeue(MQ **queue, int *elmt)
{
    printf("MQ_dequeue\n");
    MQ *p = NULL;
    MQ *next = NULL;

    if (!queue || !*queue)
        return -1;

    p = *queue;
    *elmt = p->c;

    if (p->next)
        next = p->next;

    *queue = next;
    free(*queue);
    return 0;
}

int MQ_peek(MQ *queue)
{
    printf("MQ_peek\n");

    if (queue)
        return 1;
    else
        return 0;
}

void *kb_input(void *arg)
{
    MQ *q = (MQ *) arg;

    while (1) {
        int c = (int) getchar();

        if (c)
            MQ_enqueue(q, c);
    }

    return NULL;
}

void *event_loop(void *arg)
{
    MQ *q = (MQ *) arg;
    int elmt = 0;

    while (1) {
        if (MQ_peek(q)) {
            MQ_dequeue(&q, &elmt);
            printf("Got new event: %d\n", elmt);
        } else
            msleep(200);
    }

    return NULL;
}

int main()
{
    pthread_t _kb_input;
    pthread_t _event_loop;
    pthread_create(&_kb_input, NULL, kb_input, NULL);
    pthread_create(&_event_loop, NULL, event_loop, NULL);
    pthread_join(_event_loop, NULL);
    printf("exiting main thread\n");
    return 0;
}
