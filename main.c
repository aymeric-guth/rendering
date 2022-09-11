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

void transform2d(int space[SIZE][SIZE][SIZE], int space2d[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    const float R1 = 1;
    const float R2 = 2;
    const float K2 = 5;
    const float K1 = (float)SCREEN_WIDTH * K2 * 3 / (8 * (R1 + R2));
    float zbuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    float offset = SIZE / 2.f;

    for (int k = 0; k < SIZE; k++) {
        for (int j = 0; j < SIZE; j++) {
            for (int i = 0; i < SIZE; i++) {
                float x = i - offset;
                float y = j - offset;
                float z = k - offset + K2;
                float ooz = (!z) ? (float)SIZE : 1.f / z;
                int xp = (int)(((float)SCREEN_WIDTH / 2.f) + (K1 * ooz * x));
                int yp = (int)(((float)SCREEN_HEIGHT / 2.f) - (K1 * ooz * y));
                xp = clamp(SCREEN_WIDTH - 1, 0, xp);
                yp = clamp(SCREEN_HEIGHT - 1, 0, yp);

                //xp = clamp(SCREEN_WIDTH, 0, xp);
                //yp = clamp(SCREEN_HEIGHT, 0, yp);

                //if (xp > SCREEN_WIDTH || yp > SCREEN_HEIGHT || space[k][j][i] > 12)
                //    printf("xp=%d, yp=%d, i=%d, j=%d, k=%d, x=%f, y=%f, z=%f", xp, yp, i, j, k, x, y, z);

                if (ooz > zbuffer[yp][xp]) {
                    zbuffer[yp][xp] = ooz;
                    space2d[yp][xp] = space[k][j][i];
                }
            }
        }
    }
}

void render(int space2d[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    //printf("\x1b[H");

    // printf("\e[1;1H\e[2J");
    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            int L = space2d[j][i];
            printf("%c", " .,-~:;=!*#$@"[L]);
        }

        printf("\n");
    }
}

int main()
{
    int space[SIZE][SIZE][SIZE] = {0};
    int L = 12;

    for (int i = 0; i < SIZE; i++) {
        space[0][0][i] = L;
        space[0][SIZE - 1][i] = L;
        space[SIZE - 1][0][i] = L;
        space[SIZE - 1][SIZE - 1][i] = L;
        space[i][0][0] = L;
        space[i][0][SIZE - 1] = L;
        space[i][SIZE - 1][0] = L;
        space[i][SIZE - 1][SIZE - 1] = L;
    }

    //xinit(space, 12);
    int it = -1;

    while (1) {
        int output[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
        transform2d(space, output);
        render(output);
        it = (it >= SIZE) ? 0 : it + 1;
        //xshift(space, it);
        msleep(100);
    }

    return 0;
}
