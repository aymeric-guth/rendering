#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilz.h"

#define SIZE3D 100
#define SCREEN_WIDTH 124 / 2
#define SCREEN_HEIGHT 33
#define DEBUG 0
#define CYCLES 100


typedef enum { NONE, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE } Color;

typedef struct {
    int shader;
    Color color;
} Pixel;

const char *color_map[] = {
    [NONE] = "",
    [BLACK] = "\x1b[30;1m",
    [RED] = "\x1b[31;1m",
    [GREEN] = "\x1b[32;1m",
    [YELLOW] = "\x1b[33;1m",
    [BLUE] = "\x1b[34;1m",
    [MAGENTA] = "\x1b[35;1m",
    [CYAN] = "\x1b[36;1m",
    [WHITE] = "\x1b[37;1m",
};


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

void render(Pixel space2d[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    printf("\x1b[H");

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            Pixel px = space2d[j][i];
            char c = " .,-~:;=!*#$@"[px.shader];

            if (px.color) {
                const char *color = color_map[px.color];
                printf("%s", color);
            }

            printf("%c%c\x1b[0m", c, c);
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

void transform(
    Pixel space3d[SIZE3D][SIZE3D][SIZE3D],
    Pixel space2d[SCREEN_HEIGHT][SCREEN_WIDTH],
    float zoom
)
{
    float zbuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            zbuffer[j][i] = 0.f;
    }

    // camera distance from the screen
    float K1 = 1.f;
    // screen distance from the scene
    float K2 = 1.f * zoom;

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
                    memcpy(&space2d[yp][xp], &space3d[k][j][i], sizeof(Pixel));
                    //space2d[yp][xp] = space3d[k][j][i];
                }
            }
        }
    }
}

void colorize(Pixel space3d[SIZE3D][SIZE3D][SIZE3D])
{
    for (int j = 0; j < SIZE3D; j++) {
        for (int i = 0; i < SIZE3D; i++) {
            Pixel *px = &(space3d[0][j][i]);
            px->color = RED;
            px = &space3d[SIZE3D - 1][j][i];
            px->color = CYAN;
        }
    }

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            Pixel *px = &space3d[k][j][1];
            px->color = GREEN;
            px = &space3d[k][j][SIZE3D - 1];
            px->color = BLUE;
        }
    }

    for (int k = 0; k < SIZE3D; k++) {
        for (int i = 0; i < SIZE3D; i++) {
            Pixel *px = &space3d[k][0][i];
            px->color = WHITE;
            px = &space3d[k][SIZE3D - 1][i];
            px->color = YELLOW;
        }
    }
}

int main()
{
    Pixel space3d[SIZE3D][SIZE3D][SIZE3D];
    Pixel space2d[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++) {
                Pixel px = {.color = NONE, .shader = 1};
                space3d[k][j][i] = px;
            }
        }
    }

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            Pixel px = {.color = NONE, .shader = 0};
            space2d[j][i] = px;
        }
    }

    colorize(space3d);
    // transform(space3d, space2d);
    // return 0;

    for (int n = 0; n < CYCLES; n++) {
        // moveline(space3d, n);
        transform(space3d, space2d, 3.f);
        render(space2d);
        msleep(17);
    }

    return 0;
}
