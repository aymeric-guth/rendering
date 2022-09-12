#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>

#include "constants.h"
#include "utilz.h"

void clearscreen(void)
{
    //printf("\033[XA"); // Move up X lines;
    //printf("\033[XB"); // Move down X lines;
    //printf("\033[XC"); // Move right X column;
    //printf("\033[XD"); // Move left X column;
    //printf("\033[2J"); // Clear screen
    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH * 2; i++) {
            printf("\x1b[H");
            printf("\033[%d;%dH", j, i);
            printf("^");
        }
    }
}

void render(Pixel_A *space2d)
{
    //printf("\033[XA"); // Move up X lines;
    //printf("\033[XB"); // Move down X lines;
    //printf("\033[XC"); // Move right X column;
    //printf("\033[XD"); // Move left X column;
    //printf("\033[2J"); // Clear screen
    Pixel_A *px = space2d;

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH * 2; i++) {
            printf("\x1b[H");
            printf("\033[%d;%dH", j, i);
            printf(" ");
        }
    }

    while (px) {
        //printf("\x1b[H");
        //printf("\033[%dC", (int)px->x);
        //printf("\033[%dB", (int)px->y);
        char c = " .,-~:;=!*#$@"[px->shader];
        const char *color = color_map[px->color];
        // enable color
        printf("%s", color);
        // move cursor to (x, y)
        printf("\033[%d;%dH", (int)px->y, (int)px->x * 2);
        printf("%c", c);
        printf("\033[%d;%dH", (int)px->y, (int)px->x * 2 + 1);
        printf("%c", c);
        // disable color
        printf("\x1b[0m");
        px++;
    }
}

void transform(Pixel_A *space3d, Pixel_A *space2d, float zoom)
{
    float zbuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            zbuffer[j][i] = 0.f;
    }

    // camera distance from the screen
    float K1 = 1.f;
    // screen distance from the scene
    float K2 = 1.f + zoom;

    for (int i = 0; i < SIZE3D * SIZE3D * SIZE3D; i++) {
        float ooz = 1.f / (space3d->z + K1 + K2);
        float x1 = space3d->x * K1 * ooz;
        float y1 = space3d->y * K1 * ooz;
        int xp = (int)((float)SCREEN_WIDTH / 2 + x1);
        int yp = (int)((float)SCREEN_HEIGHT / 2 + y1);

        // OOB check
        if (xp > (SCREEN_WIDTH - 1) || xp < 0 || yp > (SCREEN_HEIGHT - 1) || yp < 0) {
            space3d++;
            continue;
        }

        if (ooz > zbuffer[yp][xp]) {
            zbuffer[yp][xp] = ooz;
            space2d->x = (float)xp;
            space2d->y = (float)yp;
            space2d->z = 0.f;
            space2d->color = space3d->color;
            space2d->shader = space3d->shader;
            space2d++;
        }

        space3d++;
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
    Pixel_A *p = NULL;
    Pixel_A *pr = NULL;
    p = (Pixel_A *) malloc(sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);

    if (!p)
        return 1;

    pr = (Pixel_A *) malloc(sizeof(Pixel_A) * SCREEN_HEIGHT * SCREEN_WIDTH);

    if (!pr)
        goto cleanup;

    memset(p, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    memset(p, 0, sizeof(Pixel_A) * SCREEN_WIDTH * SCREEN_HEIGHT);

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++) {
                Pixel px = {.color = NONE, .shader = 1};
                space3d[k][j][i] = px;
            }
        }
    }

    colorize(space3d);
    {
        Pixel_A *px = p;

        for (int k = 0; k < SIZE3D; k++) {
            for (int j = 0; j < SIZE3D; j++) {
                for (int i = 0; i < SIZE3D; i++) {
                    px->x = (float)i - (float)SIZE3D / 2;
                    px->y = (float)j - (float)SIZE3D / 2;
                    px->z = (float)k;
                    px->color = space3d[k][j][i].color;
                    px->shader = space3d[k][j][i].shader;
                    px++;
                }
            }
        }
    }
    // clear screen
    printf("\033[2J");
    // hide cursor
    // printf("\e[?25l");
    float zoom = 1.f;

    for (int n = 0; n < CYCLES; n++) {
        transform(p, pr, zoom);
        clearscreen();
        render(pr);
        msleep(20);
        zoom += 1.f;
        memset(pr, 0, sizeof(Pixel_A) * SCREEN_WIDTH * SCREEN_HEIGHT);
    }

    goto cleanup;
cleanup:

    if (p)
        free(p);

    if (pr)
        free(pr);

    // show cursor
    printf("\e[?25h");
    return 0;
}
