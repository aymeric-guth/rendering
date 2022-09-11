#include <math.h>
#include <stdio.h>

#include "utilz.h"

#define SIZE3D 100
#define SCREEN_WIDTH 124 / 2
#define SCREEN_HEIGHT 33
#define DEBUG 0

void moveline(int space3d[SIZE3D][SIZE3D][SIZE3D], int n)
{
    int L = 1;

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++)
                space3d[k][j][i] = 0;

            space3d[k][j][n] = L;
            //for (int i = n; i < SIZE3D; i++)
            //    space3d[k][j][i] = L;
        }
    }
}

void _render(int space2d[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    printf("\x1b[H");

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            int L = space2d[j][i];
            printf("%c", " .,-~:;=!*#$@"[L]);
        }

        printf("\n");
    }
}

void render(int space2d[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    printf("\x1b[H");

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            int L = space2d[j][i];
            printf("\x1b[31;1m");
            printf("%c", " .,-~:;=!*#$@"[L]);
            printf("%c", " .,-~:;=!*#$@"[L]);
            printf("\x1b[0m");
        }

        printf("\n");
    }
}

float linear_interpolation(
    float src_min,
    float src_max,
    float dst_min,
    float dst_max,
    float x
)
{
    return (x - src_min) * (dst_max - dst_min) / (src_max - src_min) + dst_min;
}


void transform(int space3d[SIZE3D][SIZE3D][SIZE3D], int space2d[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    float zbuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            zbuffer[j][i] = 0.f;
    }

    // camera distance from the screen
    float K1 = 1.f;
    // screen distance from the scene
    float K2 = 10.f;

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++) {
                float x = (float)i - ((float)SIZE3D / 2);
                float y = -1.f * ((float)j - ((float)SIZE3D / 2));
                float z = (float)k;
                float ooz = 1.f / (z + K1 + K2);
                float x1 = x * K1 * ooz;
                float y1 = y * K1 * ooz;
                int xp = (int)((float)SCREEN_WIDTH / 2 + x1);
                int yp = (int)((float)SCREEN_HEIGHT / 2 + y1);

                // OOB check
                if (xp > (SCREEN_WIDTH - 1) || xp < 0 || yp > (SCREEN_HEIGHT - 1) || yp < 0) {
                    continue;

                    if (DEBUG)
                        printf("x=%f, y=%f, z=%f, ooz=%f\nx1=%f, y1=%f, xp=%d, yp=%d\n\n", x, y, z, ooz, x1, y1, xp, yp);
                }

                if (ooz > zbuffer[yp][xp]) {
                    zbuffer[yp][xp] = ooz;
                    space2d[yp][xp] = space3d[k][j][i];
                }
            }
        }
    }
}

int main()
{
    int space3d[SIZE3D][SIZE3D][SIZE3D];
    int space2d[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++)
                space3d[k][j][i] = 1;
        }
    }

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            space2d[j][i] = 0;
    }

    // transform(space3d, space2d);
    // return 0;

    for (int n = 0; n < SIZE3D; n++) {
        // moveline(space3d, n);
        transform(space3d, space2d);
        render(space2d);
        msleep(100);
    }

    return 0;
}
