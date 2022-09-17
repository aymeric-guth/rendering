#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <math.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include "fifo.h"
#include "types.h"
#include "constants.h"
#include "utilz.h"
#include "matrix.h"

void clearscreen(void)
{
    //printf("\033[XA"); // Move up X lines;
    //printf("\033[XB"); // Move down X lines;
    //printf("\033[XC"); // Move right X column;
    //printf("\033[XD"); // Move left X column;
    //printf("\033[2J"); // Clear screen
    for (int j = 0; j < TERMY; j++) {
        for (int i = 0; i < TERMX * 2; i++) {
            // printf("\x1b[H");
            // move cursor to (x, y)
            printf("\033[%d;%dH", j, i);
            printf(" ");
        }
    }

    // printf("\033[2J");
}

void render(Pixel *framebuff[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    Pixel *px = NULL;

    for (int j = 0; j < TERMY; j++) {
        for (int i = 0; i < TERMX; i++) {
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

void projection(Pixel_A *i, Pixel_A *o, Transform_Vars *tf)
{
    tf->theta = PI * 0.5f;
    float a = (float)TERMY / (float)TERMX;
    float fov = 1 / tanf(tf->theta * 0.5f);
    // zfar = max rendering distance
    float viewing_distance = 200.f;
    // znear = distance from camera to screen
    float focal_distance = 10.f;
    float q = viewing_distance / (viewing_distance - focal_distance);
    float projection_mat[4][4];
    projection_mat[0][0] = a * fov;
    projection_mat[1][0] = 0.f;
    projection_mat[2][0] = 0.f;
    projection_mat[3][0] = 0.f;
    projection_mat[0][1] = 0.f;
    projection_mat[1][1] = fov;
    projection_mat[2][1] = 0.f;
    projection_mat[3][1] = 0.f;
    projection_mat[0][2] = 0.f;
    projection_mat[1][2] = 0.f;
    projection_mat[2][2] = q;
    projection_mat[3][2] = -focal_distance * q;
    projection_mat[0][3] = 0.f;
    projection_mat[1][3] = 0.f;
    projection_mat[2][3] = 1.f;
    projection_mat[3][3] = 0.f;
    {
        o->x = i->x * projection_mat[0][0] + i->y * projection_mat[1][0] + i->z * projection_mat[2][0] + projection_mat[3][0];
        o->y = i->x * projection_mat[0][1] + i->y * projection_mat[1][1] + i->z * projection_mat[2][1] + projection_mat[3][1];
        o->z = i->x * projection_mat[0][2] + i->y * projection_mat[1][2] + i->z * projection_mat[2][2] + projection_mat[3][2];
        float w = i->x * projection_mat[0][3] + i->y * projection_mat[1][3] + i->z * projection_mat[2][3] + projection_mat[3][3];

        if (w != 0.f) {
            float oow = 1.f / w;
            o->x = o->x * oow;
            o->y = o->y * oow;
            o->z = o->z * oow;
        }
    }
}

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
        tf->z_ofst -= TRANSLATION_STEP;
        break;

    // D
    case 68:
        break;

    // W
    case 87:
        tf->z_ofst += TRANSLATION_STEP;
        break;

    // h
    case 104:
        tf->x_ofst -= TRANSLATION_STEP;
        break;

    // j
    case 106:
        tf->y_ofst -= TRANSLATION_STEP;
        break;

    // k
    case 107:
        tf->y_ofst += TRANSLATION_STEP;
        break;

    // l
    case 108:
        tf->x_ofst += TRANSLATION_STEP;
        break;
    }
}

void diry_allocator(Pixel_A *p)
{
    Pixel space3d[SIZE3D][SIZE3D][SIZE3D];

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++) {
                Pixel px = {.color = COLOR_NONE, .shader = 1};
                space3d[k][j][i] = px;
            }
        }
    }

    {
        Pixel *px = NULL;

        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++) {
                px = &(space3d[0][j][i]);
                px->color = COLOR_RED;
                px = &space3d[SIZE3D - 1][j][i];
                px->color = COLOR_CYAN;
            }
        }

        for (int k = 0; k < SIZE3D; k++) {
            for (int j = 0; j < SIZE3D; j++) {
                px = &space3d[k][j][0];
                px->color = COLOR_GREEN;
                px = &space3d[k][j][SIZE3D - 1];
                px->color = COLOR_BLUE;
            }
        }

        for (int k = 0; k < SIZE3D; k++) {
            for (int i = 0; i < SIZE3D; i++) {
                px = &space3d[k][0][i];
                px->color = COLOR_WHITE;
                px = &space3d[k][SIZE3D - 1][i];
                px->color = COLOR_YELLOW;
            }
        }
    }

    // conversion space3d static array -> heap allocated array of vector
    // TODO
    {
        for (int k = 0; k < SIZE3D; k++) {
            for (int j = 0; j < SIZE3D; j++) {
                for (int i = 0; i < SIZE3D; i++) {
                    p->x = (float)i - (float)SIZE3D / 2;
                    p->y = (float)j - (float)SIZE3D / 2;
                    p->z = (float)k - (float)SIZE3D / 2;
                    p->color = space3d[k][j][i].color;
                    p->shader = space3d[k][j][i].shader;
                    p++;
                }
            }
        }
    }
}

int entrypoint(Game_State *state)
{
    Q *q = state->q;
    Pixel_A *p = state->p;
    Pixel_A *p1 = state->p1;
    Pixel *fb = state->fb;
    Pixel *framebuff[SCREEN_HEIGHT][SCREEN_WIDTH];
    // TODO
    diry_allocator(p);
    float zbuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    {
        Pixel *fbp = fb;

        for (int j = 0; j < TERMY; j++)
            for (int i = 0; i < TERMX; i++) {
                framebuff[j][i] = fbp;
                fbp++;
            }
    }
    // clear screen
    printf("\033[2J");
    // hide cursor
    printf("\e[?25l");
    Transform_Vars tf = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    float translation_ofst = 100.0f;

    for (int n = 0; n < CYCLES; n++) {
        memcpy(p1, p, sizeof(Pixel_A)*SPACE);
        int elmt = 0;

        while (Q_get(q, &elmt) > 0)
            process_input(&tf, elmt);

        {
            for (int j = 0; j < TERMY; j++) {
                for (int i = 0; i < TERMX; i++)
                    zbuffer[j][i] = 0.f;
            }

            Pixel_A *px = p1;

            // transformation pipeline
            for (int i = 0; i < SPACE; i++) {
                // yaw
                {
                    float cos_alpha = cosf(tf.alpha);
                    float sin_alpha = sinf(tf.alpha);
                    // float yaw_mat[3][3] = {0.f};
                    // yaw_mat[0][0] = cos_alpha;
                    // yaw_mat[1][0] = sin_alpha;
                    // yaw_mat[2][0] = 0.f;
                    // yaw_mat[0][1] = -sin_alpha;
                    // yaw_mat[1][1] = cos_alpha;
                    // yaw_mat[2][1] = 0.f;
                    // yaw_mat[0][2] = 0.f;
                    // yaw_mat[1][2] = 0.f;
                    // yaw_mat[2][2] = 1.f;
                    // mat3x3(px, px, yaw_mat);
                    float x = px->x;
                    float y = px->y;
                    px->x = x * cos_alpha - (y * sin_alpha);
                    px->y = x * sin_alpha + y * cos_alpha;
                }
                // pitch
                {
                    float cos_beta = cosf(tf.beta);
                    float sin_beta = sinf(tf.beta);
                    float x = px->x;
                    float z = px->z;
                    px->x = x * cos_beta + z * sin_beta;
                    px->z = -(x * sin_beta) + z * cos_beta;
                }
                // roll
                {
                    float cos_gamma = cosf(tf.gamma);
                    float sin_gamma = sinf(tf.gamma);
                    float y = px->y;
                    float z = px->z;
                    px->y = y * cos_gamma + -(z * sin_gamma);
                    px->z = y * sin_gamma + z * cos_gamma;
                }
                // translation
                {
                    px->x += tf.x_ofst;
                    px->y += tf.y_ofst;
                    px->z += tf.z_ofst + translation_ofst;
                }
                // rectilinear projection
                Pixel_A vec;
                Pixel_A *o = &vec;
                projection(px, o, &tf);
                {
                    o->x += 1.f;
                    o->y += 1.f;
                    o->x *= 0.5f * (float)TERMX;
                    o->y *= 0.5f * (float)TERMY;
                }
                {
                    int x = (int)o->x;
                    int y = (int)o->y;

                    if (x > (TERMX - 1) || x < 0 || y > (TERMY - 1) || y < 0) {
                        px++;
                        continue;
                    }

                    if (o->z > zbuffer[y][x]) {
                        zbuffer[y][x] = o->z;
                        framebuff[y][x]->shader = px->shader;
                        framebuff[y][x]->color = px->color;
                    }
                }
                px++;
            }

            render(framebuff);
            msleep(20);
            // re-init frame-buffer
            {
                for (int j = 0; j < TERMY; j++)
                    for (int i = 0; i < TERMX; i++) {
                        framebuff[j][i]->shader = 0;
                        framebuff[j][i]->color = COLOR_NONE;
                    }
            }
        }
    }

    return 0;
}

int main()
{
    // runtime terminal size
    // struct winsize w;
    // ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    // TERMX = w.ws_row / 2;
    // TERMY = w.ws_col;
    // if (TERMX <= 0 || TERMY <= 0)
    //     goto cleanup;
    TERMX = SCREEN_WIDTH;
    TERMY = SCREEN_HEIGHT;
    // tty raw mode, non buffered io
    struct termios mode;
    tcgetattr(0, &mode);
    mode.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &mode);
    // io thread init
    pthread_t _kb_input;
    void *qp = malloc(sizeof(void *) * QUEUE_SIZE);

    if (!qp)
        goto cleanup;

    Q q = {0, 0, QUEUE_SIZE, qp};
    pthread_create(&_kb_input, NULL, kb_input, &q);
    // world init
    Pixel_A *p = (Pixel_A *) malloc(sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);

    if (!p)
        goto cleanup;

    Pixel_A *p1 = (Pixel_A *) malloc(sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);

    if (!p1)
        goto cleanup;

    Pixel *fb = (Pixel *) malloc(sizeof(Pixel) * TERMX * TERMY);

    if (!fb)
        goto cleanup;

    Pixel **framebuff = (Pixel **) malloc(sizeof(Pixel *) * TERMY);

    for (int i = 0; i < TERMY; i++) {
        Pixel *pfb = (Pixel *) malloc(sizeof(Pixel) * TERMX);
        memset(pfb, 0, sizeof(Pixel) * TERMX);
        framebuff[i] = pfb;
    }

    memset(p, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    memset(p1, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    memset(fb, 0, sizeof(Pixel) * TERMX * TERMY);
    Game_State state = { &q, p, p1, fb};
    entrypoint(&state);
    goto cleanup;
cleanup:

    if (qp)
        free(qp);

    if (p)
        free(p);

    if (p1)
        free(p1);

    if (fb)
        free(fb);

    if (framebuff) {
        while (1) {
            if (*framebuff) {
                free(*framebuff);
                framebuff++;
            } else
                break;
        }
    }

    clearscreen();
    // show cursor
    printf("\e[?25h");
    return 0;
}
