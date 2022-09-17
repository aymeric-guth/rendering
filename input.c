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
    float ROT_STEP = PI / 100.f;
    float TRANSLATION_STEP = 0.05f;
    // float ROT_STEP = PI / 75.f;
    // float TRANSLATION_STEP = 1.f;

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
        tf->v->z -= TRANSLATION_STEP;
        break;

    case KEY_D:
        break;

    case KEY_W:
        tf->v->z += TRANSLATION_STEP;
        break;

    case KEY_h:
        tf->v->x -= TRANSLATION_STEP;
        break;

    case KEY_j:
        tf->v->y -= TRANSLATION_STEP;
        break;

    case KEY_k:
        tf->v->y += TRANSLATION_STEP;
        break;

    case KEY_l:
        tf->v->x += TRANSLATION_STEP;
        break;
    }
}
