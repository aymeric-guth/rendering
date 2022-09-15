#include <stdio.h>
#include <string.h>

#include "fifo.h"
#define QUEUE_SIZE 16

int Q_get(Q *q, int *retval)
{
    if (q->tail == q->head)
        return -1;

    memcpy(&retval, &(q->data[q->head]), sizeof(int));
    memset(&(q->data[q->tail]), 0, sizeof(int));
    q->data[q->tail] = 0;
    q->tail = (q->tail + 1) % q->size;
    return 0;
}

int Q_put(Q *q, int handle)
{
    if (((q->head + 1) % q->size) == q->tail)
        return -1;

    memcpy(&(q->data[q->head]), &handle, sizeof(int));
    q->head = (q->head + 1) % q->size;
    return 0;
}

// int main(int argc, char **argv)
// {
//     Q q = {0, 0, QUEUE_SIZE, malloc(sizeof(void *) * QUEUE_SIZE)};
//
//     for (int i = 0; i < QUEUE_SIZE; i++) {
//         int res = Q_put(&q, (void *)(i + 1));
//         printf("Q_put %d\n", i + 1);
//     }
//
//     for (int i = 0; i < QUEUE_SIZE; i++) {
//         void *handle = Q_get(&q);
//         printf("Q_get %d\n", handle);
//     }
// }
