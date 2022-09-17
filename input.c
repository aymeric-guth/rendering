#include <stdio.h>
#include "input.h"
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

void process_input(Transform_Vars *tf, int elmt)
{
    float ROT_STEP = PI / 75.f;
    float TRANSLATION_STEP = 1.f;

    switch (elmt) {
    // a
    case 97:
        tf->beta -= ROT_STEP;
        break;

    // w
    case 119:
        tf->gamma += ROT_STEP;
        break;

    // s
    case 115:
        tf->gamma -= ROT_STEP;
        break;

    // d
    case 100:
        tf->beta += ROT_STEP;
        break;

    // A
    case 65:
        break;

    // S
    case 83:
        tf->v->z -= TRANSLATION_STEP;
        break;

    // D
    case 68:
        break;

    // W
    case 87:
        tf->v->z += TRANSLATION_STEP;
        break;

    // h
    case 104:
        tf->v->x -= TRANSLATION_STEP;
        break;

    // j
    case 106:
        tf->v->y -= TRANSLATION_STEP;
        break;

    // k
    case 107:
        tf->v->y += TRANSLATION_STEP;
        break;

    // l
    case 108:
        tf->v->x += TRANSLATION_STEP;
        break;
    }
}
