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
#include "constants.h"
#include "types.h"
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

// void draw(Pixel **framebuff)
// {
//     Pixel *px;
//
//     for (int j = 0; j < TERMY; j++) {
//         for (int i = 0; i < TERMX; i++) {
//             px = &framebuff[j][i];
//             char c = " .,-~:;=!*#$@"[px->shader];
//             const char *color = color_map[px->color];
//             // enable color
//             printf("%s", color);
//             // move cursor to (x, y)
//             printf("\033[%d;%dH", j, i * 2);
//             printf("%c", c);
//             // printf("\033[%d;%dH", j, i * 2 + 1);
//             // printf("%c", c);
//             printf("\x1b[0m");
//         }
//     }
// }

void draw(int x, int y, Color c, int shader)
{
    if (x < TERMX && x >= 0 && y < TERMY && y >= 0) {
        const char *color = color_map[c];
        // enable color
        printf("%s", color);
        // move cursor to (x, y)
        printf("\033[%d;%dH", y, x * 2);
        printf("%c", " .,-~:;=!*#$@"[shader]);
        printf("\033[%d;%dH", y, x * 2 + 1);
        printf("%c", c);
        printf("\x1b[0m");
    }
}

void drawline(int x1, int y1, int x2, int y2, Color c, int shader)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = abs(dx);
    dy1 = abs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;

    if (dy1 <= dx1) {
        if (dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        } else {
            x = x2;
            y = y2;
            xe = x1;
        }

        draw(x, y, c, shader);

        for (i = 0; x < xe; i++) {
            x = x + 1;

            if (px < 0)
                px = px + 2 * dy1;
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    y = y + 1;
                else
                    y = y - 1;

                px = px + 2 * (dy1 - dx1);
            }

            draw(x, y, c, shader);
        }
    } else {
        if (dy >= 0) {
            x = x1;
            y = y1;
            ye = y2;
        } else {
            x = x2;
            y = y2;
            ye = y1;
        }

        draw(x, y, c, shader);

        for (i = 0; y < ye; i++) {
            y = y + 1;

            if (py <= 0)
                py = py + 2 * dx1;
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    x = x + 1;
                else
                    x = x - 1;

                py = py + 2 * (dx1 - dy1);
            }

            draw(x, y, c, shader);
        }
    }
}

void get_proj_mat(Render_Params *params, mat4x4 mat)
{
    // rectilinear projection
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
    float **zbuff = state->zbuff;
    Q *q = state->q;
    Mesh *mesh = state->mesh;
    Transform_Vars *tf = params->tf;
    // clear screen
    printf("\033[2J");
    // hide cursor
    printf("\e[?25l");

    for (;;) {
        // for (int n = 0; n < CYCLES; n++) {
        get_term_size(params->term);
        int termx = (int) params->term->x;
        int termy = (int) params->term->y;
        int elmt = 0;

        while (Q_get(q, &elmt) > 0)
            process_input(tf, elmt);

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
        Vec3 v1 = { .x = 1.f, .y = 1.f, .z = 0.f };

        for (int j = 0; j < termy; j++) {
            for (int i = 0; i < termx; i++) {
                // re-init zbuffer
                zbuff[j][i] = 0.f;
            }
        }

        // transformation pipeline
        printf("\033[2J");

        // clearscreen();
        for (int j = 0; j < mesh->s; j++) {
            Tri tri = *mesh[j].t;
            Color c = * (Color *)mesh[j].c;

            for (int i = 0; i < 3; i++) {
                Vec3 *v = &tri.v[i];
                mat3x3_Vec3_mul(yaw_mat, v, v);
                mat3x3_Vec3_mul(pitch_mat, v, v);
                mat3x3_Vec3_mul(roll_mat, v, v);
                mat4x3_Vec3_mul(tr_mat, v, v);
                Vec3_add(v, &v0, v);
                mat4x4_Vec3_mul(proj_mat, v, v);
                {
                    Vec3_add(v, &v1, v);
                    v->x *= 0.5f * termx;
                    v->y *= 0.5f * termy;
                }
            }

            // rasterization ...
            int x1 = (int)tri.v[0].x;
            int y1 = (int)tri.v[0].y;
            int x2 = (int)tri.v[1].x;
            int y2 = (int)tri.v[1].y;
            int x3 = (int)tri.v[2].x;
            int y3 = (int)tri.v[2].y;
            drawline(x1, y1, x2, y2, c, 1);
            drawline(x2, y2, x3, y3, c, 1);
            drawline(x3, y3, x1, y1, c, 1);
        }

        msleep(20);
    }

    return 0;
}

int main()
{
    Vec2 term;
    get_term_size(&term);

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
    Mesh *m = malloc(sizeof(Mesh) * SCENE_SIZE);

    if (!m)
        goto cleanup;

    memset(m, 0, sizeof(Mesh) * SCENE_SIZE);
    m->s = SCENE_SIZE;

    for (int i = 0; i < m->s; i++) {
        m[i].t = &_scene[i];
        m[i].c = (void *)&_colors[i]->color;
    }

    float **zbuff = (float **)malloc(sizeof(float *) * term.y);

    for (int i = 0; i < term.y; i++) {
        float *pzb = (float *) malloc(sizeof(float) * term.x);
        memset(pzb, 0, sizeof(float) * term.x);
        zbuff[i] = pzb;
    }

    Game_State state = {
        .q = &q,
        .zbuff = zbuff,
        .mesh = m,
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

    if (m)
        free(m);

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
