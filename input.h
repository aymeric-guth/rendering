#ifndef INPUT_H
#define INPUT_H

#include "types.h"

typedef enum {
    KEY_a = 97,
    KEY_w = 119,
    KEY_s = 115,
    KEY_d = 100,
    KEY_A = 65,
    KEY_S = 83,
    KEY_D = 68,
    KEY_W = 87,
    KEY_h = 104,
    KEY_j = 106,
    KEY_J = 74,
    KEY_k = 107,
    KEY_K = 75,
    KEY_l = 108,
} KEY;

void *kb_input(void *arg);
void process_input(Transform_Vars *tf, int elmt);

#endif
