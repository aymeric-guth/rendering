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

void draw(Pixel **framebuff)
{
    Pixel *px;
    printf("\033[0;0H");
    char *shader = " .,-~:;=!*#$@";

    for (int j = 0; j < TERMY; j++) {
        for (int i = 0; i < TERMX; i++) {
            px = &framebuff[j][i];

            if (px->shader > 0)
                printf("\033[%d;%dH%s%c \x1b[0m", j, i * 2, color_map[px->color], shader[px->shader]);
            else
                printf("\033[%d;%dH  ", j, i * 2);

            px->shader = 0;
            px->color = COLOR_NONE;
        }

        // msleep(10);
    }
}

void drawfb(Pixel **fb, int x, int y, Color c, int shader)
{
    if (x < TERMX && x >= 0 && y < TERMY && y >= 0) {
        fb[y][x].shader = shader;
        fb[y][x].color = c;
    }
}

void drawlinevec(Pixel **fb, Raster *r)
{
    // void drawline(Pixel **fb, int x1, int y1, int x2, int y2, Color c, int shader)
    int x1 = r->l->a->x;
    int y1 = r->l->a->y;
    int x2 = r->l->b->x;
    int y2 = r->l->b->y;
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

        drawfb(fb, x, y, r->c, r->shader);

        for (i = 0; x < xe; i++) {
            x++;

            if (px < 0)
                px = px + 2 * dy1;
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    y++;
                else
                    y--;

                px = px + 2 * (dy1 - dx1);
            }

            drawfb(fb, x, y, r->c, r->shader);
        }

        return;
    }

    if (dy >= 0) {
        x = x1;
        y = y1;
        ye = y2;
    } else {
        x = x2;
        y = y2;
        ye = y1;
    }

    drawfb(fb, x, y, r->c, r->shader);

    for (i = 0; y < ye; i++) {
        y++;

        if (py <= 0)
            py = py + 2 * dx1;
        else {
            if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                x++;
            else
                x--;

            py = py + 2 * (dx1 - dy1);
        }

        drawfb(fb, x, y, r->c, r->shader);
    }
}

void drawline(Pixel **fb, int x1, int y1, int x2, int y2, Color c, int shader)
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

        drawfb(fb, x, y, c, shader);

        for (i = 0; x < xe; i++) {
            x++;

            if (px < 0)
                px = px + 2 * dy1;
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    y++;
                else
                    y--;

                px = px + 2 * (dy1 - dx1);
            }

            drawfb(fb, x, y, c, shader);
        }

        return;
    }

    if (dy >= 0) {
        x = x1;
        y = y1;
        ye = y2;
    } else {
        x = x2;
        y = y2;
        ye = y1;
    }

    drawfb(fb, x, y, c, shader);

    for (i = 0; y < ye; i++) {
        y++;

        if (py <= 0)
            py = py + 2 * dx1;
        else {
            if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                x++;
            else
                x--;

            py = py + 2 * (dx1 - dy1);
        }

        drawfb(fb, x, y, c, shader);
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
    // float **zbuff = state->zbuff;
    Q *q = state->q;
    Mesh *mesh = state->mesh;
    Pixel **fb = state->framebuff;
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
        mat3x3 tmp_mat;
        mat3x3_mul(yaw_mat, pitch_mat, tmp_mat);
        mat3x3 world_mat;
        mat3x3_mul(roll_mat, tmp_mat, world_mat);

        // transformation pipeline

        for (int j = 0; j < mesh->s; j++) {
            Tri tri = *mesh[j].t;
            Color c = * (Color *)mesh[j].c;

            for (int i = 0; i < 3; i++) {
                Vec3 *v = &tri.v[i];
                mat3x3_Vec3_mul(world_mat, v, v);
                // mat3x3_Vec3_mul(yaw_mat, v, v);
                // mat3x3_Vec3_mul(pitch_mat, v, v);
                // mat3x3_Vec3_mul(roll_mat, v, v);
                mat4x3_Vec3_mul(tr_mat, v, v);
                Vec3_add(v, &v0, v);
                mat4x4_Vec3_mul(proj_mat, v, v);
                {
                    Vec3_add(v, &v1, v);
                    v->x *= 0.5f * (float)termx;
                    v->y *= 0.5f * (float)termy;
                }
            }

            // rasterization ...
            // Vec2_Int va = { .x = (int)tri.v[0].x, .y = (int)tri.v[0].y };
            // Vec2_Int vb = { .x = (int)tri.v[1].x, .y = (int)tri.v[1].y };
            // Vec2_Int vc = { .x = (int)tri.v[2].x, .y = (int)tri.v[2].y };
            // Line_Int l1 = { .a = &va, .b = &vb };
            // Line_Int l2 = { .a = &vb, .b = &vc };
            // Line_Int l3 = { .a = &vc, .b = &va };
            // Raster r1 = { .l = &l1, .c = c, .shader = 12};
            // Raster r2 = { .l = &l2, .c = c, .shader = 12};
            // Raster r3 = { .l = &l3, .c = c, .shader = 12};
            // drawlinevec(fb, &r1);
            // drawlinevec(fb, &r2);
            // drawlinevec(fb, &r3);
            int x1 = (int)tri.v[0].x;
            int y1 = (int)tri.v[0].y;
            int x2 = (int)tri.v[1].x;
            int y2 = (int)tri.v[1].y;
            int x3 = (int)tri.v[2].x;
            int y3 = (int)tri.v[2].y;
            drawline(fb, x1, y1, x2, y2, c, 12);
            drawline(fb, x2, y2, x3, y3, c, 12);
            drawline(fb, x3, y3, x1, y1, c, 12);
        }

        draw(fb);
        msleep(20);
    }

    return 0;
}
int main()
{
    Vec2 term;
    get_term_size(&term);
    int termx = TERMX;
    int termy = TERMY;

    if (termx <= 0 || termy <= 0)
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

    Pixel **framebuff = (Pixel **) malloc(sizeof(Pixel *) * termy);
    float **zbuff = (float **)malloc(sizeof(float *) * termy);

    for (int j = 0; j < termy; j++) {
        Pixel *pfb = (Pixel *) malloc(sizeof(Pixel) * termx);
        memset(pfb, 0, sizeof(Pixel) * termx);
        framebuff[j] = pfb;
        float *pzb = (float *) malloc(sizeof(float) * termx);
        memset(pzb, 0, sizeof(float) * termx);
        zbuff[j] = pzb;
    }

    Game_State state = {
        .q = &q,
        .zbuff = zbuff,
        .framebuff = framebuff,
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

    // show cursor
    printf("\e[?25h");
    return 0;
}
