#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "utilz.h"

#define SIZE 100
#define SCREEN_WIDTH 124
#define SCREEN_HEIGHT 33

int get_index(int idx, int size)
{
    if (idx < 0)
        return idx + size;
    else if (idx >= size)
        return idx - size;
    else
        return idx;
}

int yshift(int space[SIZE][SIZE][SIZE], int idx)
{
    if (idx >= SIZE || idx <= -SIZE)
        return -1;

    for (int k = 0; k < SIZE; k++) {
        for (int i = 0; i < SIZE; i++) {
            int buff[SIZE] = {0};

            for (int j = 0; j < SIZE; j++)
                buff[j] = space[k][j][i];

            for (int j = 0; j < SIZE; j++)
                space[k][get_index(j + idx, SIZE)][i] = buff[j];
        }
    }

    return 0;
}

int zshift(int space[SIZE][SIZE][SIZE], int idx)
{
    if (idx >= SIZE || idx <= -SIZE)
        return -1;

    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i < SIZE; i++) {
            int buff[SIZE] = {0};

            for (int k = 0; k < SIZE; k++)
                buff[k] = space[k][j][i];

            for (int k = 0; k < SIZE; k++)
                space[get_index(k + idx, SIZE)][j][i] = buff[k];
        }
    }

    return 0;
}

void xinit(int space[SIZE][SIZE][SIZE], int initializer)
{
    for (int k = 0; k < SIZE; k++) {
        for (int j = 0; j < SIZE; j++)
            space[k][j][0] = initializer;
    }
}



int xshift(int space[SIZE][SIZE][SIZE], int idx)
{
    if (idx >= SIZE || idx <= -SIZE)
        return -1;

    for (int k = 0; k < SIZE; k++) {
        for (int j = 0; j < SIZE; j++) {
            int buff[SIZE] = {0};

            for (int i = 0; i < SIZE; i++)
                buff[i] = space[k][j][i];

            for (int i = 0; i < SIZE; i++)
                space[k][j][get_index(i + idx, SIZE)] = buff[i];
        }
    }

    return 0;
}
