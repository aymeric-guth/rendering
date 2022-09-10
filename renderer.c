#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "utilz.h"

#define SIZE 100
#define PIXELS 13
#define SCREEN_WIDTH 124
#define SCREEN_HEIGHT 33 - 2

const char *pixels[] = {" ", ".", ",", "-", "~", ":", ";", "=", "!", "*", "#", "$", "@"};

void print_space(int space[SIZE][SIZE][SIZE])
{
    for (int k = 0; k < SIZE; k++) {
        printf("[");

        for (int j = 0; j < SIZE; j++) {
            printf("[");

            for (int i = 0; i < SIZE; i++)
                printf("%d, ", space[k][j][i]);

            printf("],\n");
        }

        printf("],\n");
    }
}


int get_index(int idx, int size)
{
    if (idx < 0)
        return idx + size;
    else if (idx >= size)
        return idx - size;
    else
        return idx;
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

void init(int space[SIZE][SIZE][SIZE], int initializer)
{
    for (int k = 0; k < SIZE; k++) {
        for (int j = 0; j < SIZE; j++)
            for (int i = 0; i < SIZE; i++)
                space[k][j][i] = initializer;
    }
}

void transform2d(int space[SIZE][SIZE][SIZE], char space2d[SCREEN_WIDTH][SCREEN_HEIGHT])
{
    const float R1 = 1;
    const float R2 = 2;
    const float K2 = 5;
    const float K1 = (float)SCREEN_WIDTH * K2 * 3 / (8 * (R1 + R2));
    float zbuffer[SCREEN_WIDTH][SCREEN_HEIGHT] = {0};
    float offset = (float) SIZE / 2;

    for (int k = 0; k < SIZE; k++) {
        for (int j = 0; j < SIZE; j++) {
            for (int i = 0; i < SIZE; i++) {
                float x = i;
                float y = j;
                float z = k;
                float ooz = 1.f / z;
                int L = space[k][j][i];
                int xp = (int)(((float)SCREEN_WIDTH / 2.f) + (K1 * ooz * x));
                int yp = (int)(((float)SCREEN_HEIGHT / 2.f) - (K1 * ooz * y));
                xp = clamp(SCREEN_WIDTH, 0, xp);
                yp = clamp(SCREEN_HEIGHT, 0, yp);
                space2d[yp][xp] = *pixels[L];
            }
        }
    }
}

void render(char space2d[SCREEN_WIDTH][SCREEN_HEIGHT])
{
    printf("\x1b[H");

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            printf("%c", space2d[j][i]);

        printf("\n");
    }
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main()
{
    int space[SIZE][SIZE][SIZE] = {0x0};
    init(space, 12);
    int i = 0;

    while (1) {
        char output[SCREEN_WIDTH][SCREEN_HEIGHT] = {' '};
        transform2d(space, output);
        render(output);

        if (i >= SIZE)
            i = 0;

        xshift(space, i);
        msleep(1);
    }

    //r =
    //r = yshift(space, 0);
    // r = zshift(space, 0);
    // if (r)
    //     return 1;
    // print_space(space);
    return 0;
}
