#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include "fifo.h"
#include "types.h"
#include "constants.h"
#include "utilz.h"
#include "matrix.h"
#include "input.h"

int TERMX = 0;
int TERMY = 0;

void get_term_size(Vec2 *v)
{
    // runtime terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    v->x = (float)w.ws_col / 2.f;
    v->y = (float)w.ws_row;
    TERMX = (int) v->x;
    TERMY = (int) v->y;
}

// TODO
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
            // move cursor to (y, x)
            printf("\033[%d;%dH", j, i);
            printf(" ");
        }
    }

    // printf("\033[2J");
}

// TODO
void draw(Pixel **framebuff)
{
    Pixel *px;

    for (int j = 0; j < TERMY; j++) {
        for (int i = 0; i < TERMX; i++) {
            px = &framebuff[j][i];
            char c = " .,-~:;=!*#$@"[px->shader];
            const char *color = color_map[px->color];
            // enable color
            printf("%s", color);
            // move cursor to (x, y)
            printf("\033[%d;%dH", j, i * 2);
            printf("%c", c);
            printf("\033[%d;%dH", j, i * 2 + 1);
            printf("%c", c);
            printf("\x1b[0m");
        }
    }
}

void get_proj_mat(Render_Params *params, mat4x4 mat)
{
    float a = (float)params->term->y / (float)params->term->x;
    float fov = 1 / tanf(params->tf->theta * 0.5f);
    float viewing_distance = params->viewing_distance;
    float focal_distance = params->focal_distance;
    float q = viewing_distance / (viewing_distance - focal_distance);
    mat[0][0] = a * fov;
    mat[1][0] = 0.f;
    mat[2][0] = 0.f;
    mat[3][0] = 0.f;
    mat[0][1] = 0.f;
    mat[1][1] = fov;
    mat[2][1] = 0.f;
    mat[3][1] = 0.f;
    mat[0][2] = 0.f;
    mat[1][2] = 0.f;
    mat[2][2] = q;
    mat[3][2] = -focal_distance * q;
    mat[0][3] = 0.f;
    mat[1][3] = 0.f;
    mat[2][3] = 1.f;
    mat[3][3] = 0.f;
}

// TODO
void Px_to_Vec3(Pixel_A *px, Vec3 *v)
{
    v->x = px->x;
    v->y = px->y;
    v->z = px->z;
}

// TODO
void Vec3_to_Px(Pixel_A *px, Vec3 *v)
{
    px->x = v->x;
    px->y = v->y;
    px->z = v->z;
}

void dirty_allocator(Pixel_A *p)
{
    Pixel space3d[SIZE3D][SIZE3D][SIZE3D];

    for (int k = 0; k < SIZE3D; k++) {
        for (int j = 0; j < SIZE3D; j++) {
            for (int i = 0; i < SIZE3D; i++) {
                Pixel px = {.color = COLOR_NONE, .shader = 12};
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

int entrypoint(Game_State *state, Render_Params *params)
{
    // TODO
    Pixel_A *p = state->p;
    Pixel_A *p1 = state->p1;
    Pixel **framebuff = state->framebuff;
    //
    Q *q = state->q;
    float **zbuff = state->zbuff;
    // TODO
    dirty_allocator(p);
    // clear screen
    printf("\033[2J");
    // hide cursor
    printf("\e[?25l");
    struct timespec start, end;

    for (int n = 0; n < CYCLES; n++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        memcpy(p1, p, sizeof(Pixel_A)*SPACE);
        int elmt = 0;
        params->tf->alpha += PI / 100;
        params->tf->beta += PI / 200;
        params->tf->gamma += PI / 300;

        while (Q_get(q, &elmt) > 0)
            process_input(params->tf, elmt);

        // re-init zbuffer
        for (int j = 0; j < params->term->y; j++) {
            for (int i = 0; i < params->term->x; i++)
                zbuff[j][i] = 0.f;
        }

        // re-init frame-buffer
        {
            for (int j = 0; j < params->term->y; j++)
                for (int i = 0; i < params->term->x; i++) {
                    framebuff[j][i].shader = 0;
                    framebuff[j][i].color = COLOR_NONE;
                }
        }
        // TODO
        Pixel_A *px = p1;
        get_term_size(params->term);
        mat4x4 proj_mat;
        get_proj_mat(params, proj_mat);
        mat3x3 yaw_mat;
        get_yaw_mat(params->tf->alpha, yaw_mat);
        mat3x3 pitch_mat;
        get_pitch_mat(params->tf->beta, pitch_mat);
        mat3x3 roll_mat;
        get_roll_mat(params->tf->gamma, roll_mat);
        mat4x3 tr_mat;
        get_tr_mat(params->tf->v, tr_mat);
        Vec3 v0 = { .x = 0.f, .y = 0.f, .z = params->translation_ofst};

        // transformation pipeline
        for (int i = 0; i < SPACE; i++) {
            Vec3 v;
            // TODO
            Px_to_Vec3(px, &v);
            mat3x3_Vec3_mul(yaw_mat, &v, &v);
            mat3x3_Vec3_mul(pitch_mat, &v, &v);
            mat3x3_Vec3_mul(roll_mat, &v, &v);
            mat4x3_Vec3_mul(tr_mat, &v, &v);
            Vec3_add(&v, &v0, &v);
            // rectilinear projection
            mat4x4_Vec3_mul(proj_mat, &v, &v);
            {
                v.x += 1.f;
                v.y += 1.f;
                v.z += 1.f;
                v.x *= 0.5f * params->term->x;
                v.y *= 0.5f * params->term->y;
                v.z *= 0.5f;
            }
            {
                if (v.x > (params->term->x - 1.f) || v.x < 0.f || v.y > (params->term->y - 1.f) || v.y < 0.f) {
                    px++;
                    continue;
                }

                // TODO
                int x = (int)v.x;
                int y = (int)v.y;

                if (v.z < zbuff[y][x]) {
                    zbuff[y][x] = v.z;
                    // TODO
                    framebuff[y][x].shader = px->shader;
                    framebuff[y][x].color = px->color;
                }
            }
            px++;
        }

        // drawing routine
        draw(framebuff);
        // compensate if rendering speed < FRAME_US ~60Hz
        // ne fonctionne pas correctement
        // clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        // uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        // if ((float)delta_us > FRAME_US) {
        //     float val = (float)delta_us - FRAME_US;
        //     msleep(val / 1000);
        // }
    }

    return 0;
}

int main()
{
    Vec2 term;
    get_term_size(&term);
    int termx = TERMX;
    int termy = TERMY;

    if (term.x <= 0.f || term.y <= 0)
        goto cleanup;

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

    Pixel *fb = (Pixel *) malloc(sizeof(Pixel) * termx * termy);

    if (!fb)
        goto cleanup;

    Pixel **framebuff = (Pixel **) malloc(sizeof(Pixel *) * termy);
    float **zbuff = (float **)malloc(sizeof(float *) * termy);

    for (int i = 0; i < termy; i++) {
        Pixel *pfb = (Pixel *) malloc(sizeof(Pixel) * termx);
        memset(pfb, 0, sizeof(Pixel) * termx);
        framebuff[i] = pfb;
        float *pzb = (float *) malloc(sizeof(float) * termx);
        memset(pzb, 0, sizeof(float) * termx);
        zbuff[i] = pzb;
    }

    memset(p, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    memset(p1, 0, sizeof(Pixel_A) * SIZE3D * SIZE3D * SIZE3D);
    Game_State state = { .q = &q, .p = p, .p1 = p1, .framebuff = framebuff, .zbuff = zbuff };
    Vec3 ofst = {.x = 0.f, .y = 0.f, .z = 0.f};
    Transform_Vars tf = {
        .alpha = 0.f,
        .beta = 0.f,
        .gamma = 0.f,
        .theta = PI * 0.5f,
        .v = &ofst
    };
    Render_Params params = {
        .tf = &tf,
        .term = &term,
        .focal_distance = 1000.f,
        .translation_ofst = 120.f,
        .viewing_distance = 10000.f
    };
    entrypoint(&state, &params);
    goto cleanup;
cleanup:

    if (qp)
        free(qp);

    if (p)
        free(p);

    if (p1)
        free(p1);

    if (framebuff) {
        while (1) {
            if (*framebuff) {
                free(*framebuff);
                framebuff++;
            } else
                break;
        }
    }

    if (zbuff) {
        while (1) {
            if (*zbuff) {
                free(*zbuff);
                zbuff++;
            } else
                break;
        }
    }

    clearscreen();
    // show cursor
    printf("\e[?25h");
    return 0;
}
