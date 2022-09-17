#include <math.h>
#include <stdint.h>
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
#include "input.h"

int TERMX = 0;
int TERMY = 0;

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
            printf("\033[%d;%dH", j, i * 2 + 1);
            printf("%c", c);
            printf("\x1b[0m");
        }
    }
}

void get_proj_mat(Render_Params *params, mat4x4 mat)
{
    float a = params->termy / params->termx;
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

void Px_to_Vec3(Pixel_A *px, Vec3 *v)
{
    v->x = px->x;
    v->y = px->y;
    v->z = px->z;
}

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
    Q *q = state->q;
    Pixel_A *p = state->p;
    Pixel_A *p1 = state->p1;
    Pixel **framebuff = state->framebuff;
    float **zbuff = state->zbuff;
    // TODO
    dirty_allocator(p);
    // clear screen
    printf("\033[2J");
    // hide cursor
    printf("\e[?25l");
    Transform_Vars *tf = params->tf;

    for (int n = 0; n < CYCLES; n++) {
        memcpy(p1, p, sizeof(Pixel_A)*SPACE);
        int elmt = 0;

        while (Q_get(q, &elmt) > 0)
            process_input(tf, elmt);

        {
            for (int j = 0; j < TERMY; j++) {
                for (int i = 0; i < TERMX; i++)
                    zbuff[j][i] = 0.f;
            }

            Pixel_A *px = p1;
            mat4x4 proj_mat;
            get_proj_mat(params, proj_mat);
            mat3x3 yaw_mat;
            get_yaw_mat(params->tf->alpha, yaw_mat);
            mat3x3 pitch_mat;
            get_pitch_mat(params->tf->beta, pitch_mat);
            mat3x3 roll_mat;
            get_roll_mat(params->tf->gamma, roll_mat);

            // transformation pipeline
            for (int i = 0; i < SPACE; i++) {
                {
                    // yaw
                    Vec3 vi;
                    Vec3 vo;
                    Px_to_Vec3(px, &vi);
                    mat3x3_Vec3_mul(yaw_mat, &vi, &vo);
                    Vec3_to_Px(px, &vo);
                }
                {
                    // pitch
                    Vec3 vi;
                    Vec3 vo;
                    Px_to_Vec3(px, &vi);
                    mat3x3_Vec3_mul(pitch_mat, &vi, &vo);
                    Vec3_to_Px(px, &vo);
                }
                {
                    // roll
                    Vec3 vi;
                    Vec3 vo;
                    Px_to_Vec3(px, &vi);
                    mat3x3_Vec3_mul(roll_mat, &vi, &vo);
                    Vec3_to_Px(px, &vo);
                }
                {
                    // translation
                    Vec3 vi;
                    Vec3 vo;
                    Px_to_Vec3(px, &vi);
                    Vec3_add(&vi, tf->v, &vo);
                    Vec3 v0 = { .x = 0.f, .y = 0.f, .z = params->translation_ofst};
                    Vec3_add(&vo, &v0, &vo);
                    Vec3_to_Px(px, &vo);
                }
                {
                    // rectilinear projection
                    Vec3 vi;
                    Vec3 vo;
                    Px_to_Vec3(px, &vi);
                    mat4x4_Vec3_mul(proj_mat, &vi, &vo);
                    {
                        vo.x += 1.f;
                        vo.y += 1.f;
                        vo.x *= 0.5f * params->termx;
                        vo.y *= 0.5f * params->termy;
                    }
                    {
                        int x = (int)vo.x;
                        int y = (int)vo.y;

                        if (x > (params->termx - 1) || x < 0 || y > (params->termy - 1) || y < 0) {
                            px++;
                            continue;
                        }

                        if (vo.z < zbuff[y][x]) {
                            zbuff[y][x] = vo.z;
                            framebuff[y][x].shader = px->shader;
                            framebuff[y][x].color = px->color;
                        }
                    }
                }
                px++;
            }

            draw(framebuff);
            // msleep(20);
            // re-init frame-buffer
            {
                for (int j = 0; j < TERMY; j++)
                    for (int i = 0; i < TERMX; i++) {
                        framebuff[j][i].shader = 0;
                        framebuff[j][i].color = COLOR_NONE;
                    }
            }
        }
    }

    return 0;
}

int main()
{
    // runtime terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    TERMX = w.ws_col / 2;
    TERMY = w.ws_row;

    if (TERMX <= 0 || TERMY <= 0)
        goto cleanup;

    // TERMX = SCREEN_WIDTH;
    // TERMY = SCREEN_HEIGHT;
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
    float **zbuff = (float **)malloc(sizeof(float *) * TERMY);

    for (int i = 0; i < TERMY; i++) {
        Pixel *pfb = (Pixel *) malloc(sizeof(Pixel) * TERMX);
        memset(pfb, 0, sizeof(Pixel) * TERMX);
        framebuff[i] = pfb;
        float *pzb = (float *) malloc(sizeof(float) * TERMX);
        memset(pzb, 0, sizeof(float) * TERMX);
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
        .termx = (float)TERMX,
        .termy = (float)TERMY,
        .focal_distance = 100.f,
        .translation_ofst = 100.f,
        .viewing_distance = 1000.f
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
