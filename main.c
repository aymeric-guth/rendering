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

#include "scene_data.h"

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
            // printf("\033[%d;%dH", j, i * 2 + 1);
            // printf("%c", c);
            printf("\x1b[0m");
        }
    }
}

void get_proj_mat(Render_Params *params, mat4x4 mat)
{
    float a = (float)params->term->y / (float)params->term->x;
    float fov = 1 / tanf(params->theta * 0.5f);
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

int entrypoint_tri(Game_State *state, Render_Params *params)
{
    Pixel **framebuff = state->framebuff;
    float **zbuff = state->zbuff;
    Q *q = state->q;
    Tri *mesh = state->mesh;
    Pixel **px = state->px;
    Transform_Vars *tf = params->tf;
    // clear screen
    printf("\033[2J");
    // hide cursor
    printf("\e[?25l");

    for (int n = 0; n < CYCLES; n++) {
        int elmt = 0;

        while (Q_get(q, &elmt) > 0)
            process_input(tf, elmt);

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
        get_term_size(params->term);
        mat4x4 proj_mat;
        get_proj_mat(params, proj_mat);
        mat3x3 yaw_mat;
        get_yaw_mat(tf->alpha, yaw_mat);
        mat3x3 pitch_mat;
        get_pitch_mat(tf->beta, pitch_mat);
        mat3x3 roll_mat;
        get_roll_mat(tf->gamma, roll_mat);
        mat4x3 tr_mat;
        get_tr_mat(tf->v, tr_mat);
        Vec3 v0 = { .x = 0.f, .y = 0.f, .z = params->translation_ofst};

        // transformation pipeline
        for (int j = 0; j < SCENE_SIZE; j++) {
            for (int i = 0; i < 3; i++) {
                Vec3 v = mesh[j].v[i];
                mat3x3_Vec3_mul(yaw_mat, &v, &v);
                mat3x3_Vec3_mul(pitch_mat, &v, &v);
                mat3x3_Vec3_mul(roll_mat, &v, &v);
                mat4x3_Vec3_mul(tr_mat, &v, &v);
                Vec3_add(&v, &v0, &v);
                //
                // rasterization ...
                //
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
                    if (v.x > (params->term->x - 1.f) || v.x < 0.f || v.y > (params->term->y - 1.f) || v.y < 0.f)
                        continue;

                    // TODO
                    int x = (int)v.x;
                    int y = (int)v.y;

                    if (v.z < zbuff[y][x]) {
                        zbuff[y][x] = v.z;
                        framebuff[y][x].shader = px[j][i].shader;
                        framebuff[y][x].color = px[j][i].color;
                    }
                }
            }
        }

        // drawing routine
        draw(framebuff);
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

    Q q = { .head = 0, .tail = 0, .size = QUEUE_SIZE, .data = qp};
    pthread_create(&_kb_input, NULL, kb_input, &q);
    // world init
    Tri *ptr = malloc(sizeof(Tri) * SCENE_SIZE);

    if (!ptr)
        goto cleanup;

    memset(ptr, 0, sizeof(Tri) * SCENE_SIZE);
    memcpy(ptr, _scene, sizeof(Tri) * SCENE_SIZE);
    Pixel **ptr2 = malloc(sizeof(Pixel *) * SCENE_SIZE);

    if (!ptr2)
        goto cleanup;

    memset(ptr2, 0, sizeof(Pixel *) * SCENE_SIZE);

    for (int i = 0; i < SCENE_SIZE; i++) {
        Pixel *px = (Pixel *) malloc(sizeof(Pixel) * 3);
        memset(px, 0, sizeof(Pixel) * 3);
        memcpy(px, _colors[i], sizeof(Pixel) * 3);
        ptr2[i] = px;
    }

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

    Game_State state = {
        .q = &q,
        .framebuff = framebuff,
        .zbuff = zbuff,
        .mesh = ptr,
        .px = ptr2
    };
    Vec3 ofst = {.x = 0.f, .y = 0.f, .z = 0.f};
    Transform_Vars tf = {
        .alpha = 0.f,
        .beta = 0.f,
        .gamma = 0.f,
        .v = &ofst
    };
    Render_Params params = {
        .tf = &tf,
        .term = &term,
        .focal_distance = 10.f,
        .translation_ofst = 2.f,
        .viewing_distance = 100.f,
        .theta = PI * 0.5f
    };
    entrypoint_tri(&state, &params);
    goto cleanup;
cleanup:

    if (qp)
        free(qp);

    if (ptr)
        free(ptr);

    if (ptr2) {
        while (1) {
            if (*ptr2) {
                free(*ptr2);
                ptr2++;
            } else
                break;
        }
    }

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
