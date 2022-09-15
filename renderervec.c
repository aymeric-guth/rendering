#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <math.h>
#include <ncurses.h>
#include "pthread.h"

#include "fifo.h"
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
            // printf("\x1b[H");
            // move cursor to (x, y)
            printf("\033[%d;%dH", j, i);
            printf(" ");
        }
    }

    // printf("\033[2J");
}

void colorize(Pixel space3d[SIZE3D][SIZE3D][SIZE3D])
{
    Pixel *px = NULL;

    for (int j = 0; j < SIZE3D; j++) {
        for (int i = 0; i < SIZE3D; i++) {
            px = &(space3d[0][j][i]);
            px->color = RED;
            px = &space3d[SIZE3D - 1][j][i];
            px->color = CYAN;
        }
    }

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            px = &space3d[k][j][0];
            px->color = GREEN;
            px = &space3d[k][j][SIZE3D - 1];
            px->color = BLUE;
        }
    }

    for (int k = 0; k < SIZE3D; k++) {
        for (int i = 0; i < SIZE3D; i++) {
            px = &space3d[k][0][i];
            px->color = WHITE;
            px = &space3d[k][SIZE3D - 1][i];
            px->color = YELLOW;
        }
    }
}

void rendervec(Pixel_A *framebuff)
{
    Pixel_A *px = framebuff;

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
        // printf("\033[%d;%dH", (int)px->y, (int)px->x * 2 + 1);
        // printf("%c", c);
        // disable color
        printf("\x1b[0m");
        px++;
    }
}

void render(Pixel *framebuff[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    Pixel *px = NULL;

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            px = framebuff[j][i];
            char c = " .,-~:;=!*#$@"[px->shader];
            const char *color = color_map[px->color];
            // enable color
            printf("%s", color);
            // move cursor to (x, y)
            printf("\033[%d;%dH", j, i * 2);
            printf("%c", c);
            // printf("\033[%d;%dH", (int)px->y, (int)px->x * 2 + 1);
            // disable color
            printf("\x1b[0m");
        }
    }
}

void rot_yaw(Pixel_A *vec3, float theta)
{
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);
    float x = vec3->x;
    float y = vec3->y;
    vec3->x = x * cos_theta - (y * sin_theta);
    vec3->y = x * sin_theta + y * cos_theta;
}

void rot_pitch(Pixel_A *vec3, float theta)
{
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);
    float x = vec3->x;
    float z = vec3->z;
    vec3->x = x * cos_theta + z * sin_theta;
    vec3->z = -(x * sin_theta) + z * cos_theta;
}

void rot_roll(Pixel_A *vec3, float theta)
{
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);
    float y = vec3->y;
    float z = vec3->z;
    vec3->y = y * cos_theta + -(z * sin_theta);
    vec3->z = y * sin_theta + z * cos_theta;
}

void transform(Pixel_A *space3d, Pixel *framebuff[SCREEN_HEIGHT][SCREEN_WIDTH], float zoom)
{
    float zbuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            zbuffer[j][i] = 0.f;
    }

    // camera distance from the screen ~focal distance
    const float K1 = 30.f;
    // screen distance from the scene
    const float K2 = 30.f;

    for (int i = 0; i < SIZE3D * SIZE3D * SIZE3D; i++) {
        // float z = space3d->z + K1 + K2;
        // float ooz = 1.f / z;
        // float x1 = space3d->x * ooz;
        // float y1 = space3d->y * ooz;
        // int xp = (int)((float)SCREEN_WIDTH / 2 - x1);
        // int yp = (int)((float)SCREEN_HEIGHT / 2 - y1);
        float x = space3d->x;
        float y = space3d->y;
        float z = space3d->z;
        float ooz = 1.f / (z + K1 + K2);
        float x1 = (x * K1) * ooz;
        float y1 = (y * K1) * ooz;
        int xp = (int)((float)SCREEN_WIDTH / 2 - x1);
        int yp = (int)((float)SCREEN_HEIGHT / 2 - y1);

        // OOB check
        if (xp > (SCREEN_WIDTH - 1) || xp < 0 || yp > (SCREEN_HEIGHT - 1) || yp < 0) {
            space3d++;
            continue;
        }

        if (ooz > zbuffer[yp][xp]) {
            zbuffer[yp][xp] = ooz;
            {
                Pixel *p = framebuff[yp][xp];
                p->shader = space3d->shader;
                p->color = space3d->color;
            }
        }

        space3d++;
    }
}

void init_framebuff(Pixel *framebuff[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    Pixel *p = NULL;

    for (int j = 0; j < SCREEN_HEIGHT; j++)
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            p = framebuff[j][i];
            p->shader = 0;
            p->color = NONE;
        }
}

void *kb_input(void *arg)
{
    Q *q = (Q *) arg;

    while (1) {
        int c = getch();

        if (c)
            Q_put(q, c);
    }

    return NULL;
}


int main()
{
    initscr();
    pthread_t _kb_input;
    Q q = {0, 0, QUEUE_SIZE, malloc(sizeof(void *) * QUEUE_SIZE)};
    pthread_create(&_kb_input, NULL, kb_input, &q);
    Pixel space3d[SIZE3D][SIZE3D][SIZE3D];
    Pixel *framebuff[SCREEN_HEIGHT][SCREEN_WIDTH];
    Pixel_A *p = (Pixel_A *) malloc(sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);

    if (!p)
        goto cleanup;

    Pixel_A *p1 = (Pixel_A *) malloc(sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);

    if (!p1)
        goto cleanup;

    Pixel *fb = (Pixel *) malloc(sizeof(Pixel) * SCREEN_WIDTH * SCREEN_HEIGHT);

    if (!fb)
        goto cleanup;

    memset(p, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    memset(p1, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    memset(fb, 0, sizeof(Pixel) * SCREEN_WIDTH * SCREEN_HEIGHT);
    {
        Pixel *fbp = fb;

        for (int j = 0; j < SCREEN_HEIGHT; j++)
            for (int i = 0; i < SCREEN_WIDTH; i++) {
                framebuff[j][i] = fbp;
                fbp++;
            }
    }

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
    printf("\e[?25l");
    float zoom = 200.f;
    float theta = 0.f;
    float a = 0.f;

    for (int n = 0; n < CYCLES; n++) {
        memcpy(p1, p, sizeof(Pixel_A)*SPACE);
        int elmt = 0;
        Q_get(&q, &elmt);

        if (elmt > 0) {
            Pixel_A *px = p1;

            for (int i = 0; i < SPACE; i++) {
                px->color = WHITE;
                px++;
            }
        }

        {
            Pixel_A *px = p1;

            for (int i = 0; i < SPACE; i++) {
                rot_yaw(px, theta);
                rot_pitch(px, theta);
                rot_roll(px, theta);
                px++;
            }
        }

        transform(p1, framebuff, zoom);
        render(framebuff);
        msleep(20);
        // re-init frame-buffer
        init_framebuff(framebuff);
        a += 1.f;
        theta += PI / 100;
    }

    goto cleanup;
cleanup:

    if (p)
        free(p);

    if (p1)
        free(p1);

    if (fb)
        free(fb);

    clearscreen();
    // show cursor
    printf("\e[?25h");
    endwin();
    return 0;
}
