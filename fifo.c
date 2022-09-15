#include <stdio.h>

#include "fifo.h"
#define QUEUE_SIZE 16

void *Q_get(Q *q)
{
    if (q->tail == q->head)
        return NULL;

    void *handle = q->data[q->tail];
    q->data[q->tail] = NULL;
    q->tail = (q->tail + 1) % q->size;
    return handle;
}

int Q_put(Q *q, void *handle)
{
    if (((q->head + 1) % q->size) == q->tail)
        return -1;

    q->data[q->head] = handle;
    q->head = (q->head + 1) % q->size;
    return 0;
}

int main(int argc, char **argv)
{
    Q q = {0, 0, QUEUE_SIZE, malloc(sizeof(void *) * QUEUE_SIZE)};

    for (int i = 0; i < QUEUE_SIZE; i++) {
        int res = Q_put(&q, (void *)(i + 1));
        printf("Q_put %d\n", i + 1);
    }

    for (int i = 0; i < QUEUE_SIZE; i++) {
        void *handle = Q_get(&q);
        printf("Q_get %d\n", handle);
    }
}
