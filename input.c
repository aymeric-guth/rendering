#include <stdio.h>

#include "constants.h"
#include "fifo.h"
#include "input.h"

extern int running;

void *kb_input(void *arg)
{
    Q *q = (Q *) arg;

    while (running) {
        int c = getchar();
        Q_put(q, c);
        //msleep(20);
    }

    return NULL;
}

void process_input(Transform_Vars *tf, int elmt)
{
    switch (elmt) {
    case KEY_a:
        tf->beta -= ROT_STEP;
        break;

    case KEY_w:
        tf->gamma += ROT_STEP;
        break;

    case KEY_s:
        tf->gamma -= ROT_STEP;
        break;

    case KEY_d:
        tf->beta += ROT_STEP;
        break;

    case KEY_A:
        break;

    case KEY_S:
        tf->alpha -= ROT_STEP;
        break;

    case KEY_D:
        break;

    case KEY_W:
        tf->alpha += ROT_STEP;
        break;

    case KEY_h:
        tf->v->x -= TR_STEP;
        break;

    case KEY_j:
        tf->v->y += TR_STEP;
        break;

    case KEY_k:
        tf->v->y -= TR_STEP;
        break;

    case KEY_l:
        tf->v->x += TR_STEP;
        break;

    case KEY_J:
        tf->v->z += TR_STEP;
        break;

    case KEY_K:
        tf->v->z -= TR_STEP;
        break;
    }
}
