#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "fifo.h"
#include "constants.h"
#include "types.h"
#include "matrix.h"
#include "input.h"
#include "draw.h"
#include "rasterizer.h"
#include "utilz.h"

#include "scene_data.h"

extern int TERMX;
extern int TERMY;
extern int running;

void get_term_size(Vec2 *v)
{
    // runtime terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    v->x = (float)w.ws_col * 0.5f;
    v->y = (float)w.ws_row;
    TERMX = (int) v->x;
    TERMY = (int) v->y;
}

double delta_time(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1000000000.f;
}

void mesh_center(Mesh *m, Vec3 *v)
{
    float fmax = 100.f;
    float xmin = fmax;
    float xmax = -fmax;
    float ymin = fmax;
    float ymax = -fmax;
    float zmin = fmax;
    float zmax = -fmax;

    for (int i = 0; i < m->s; i++) {
        for (int j = 0; j < 3; j++) {
            float x = m[i].t[j].v->x;
            float y = m[i].t[j].v->y;
            float z = m[i].t[j].v->z;

            if (x < xmin)
                xmin = x;

            if (x > xmax)
                xmax = x;

            if (y < ymin)
                ymin = y;

            if (y > ymax)
                ymax = y;

            if (z < zmin)
                zmin = z;

            if (z > zmax)
                zmax = z;
        }
    }

    v->x = (xmin + xmax) * -0.5f;
    v->y = (ymin + ymax) * -0.5f;
    v->z = (zmin + zmax) * -0.5f;
}

int entrypoint_tri(Game_State *state, Render_Params *params)
{
    float **zbuff = state->zbuff;
    Q *q = state->q;
    Mesh *mesh = state->mesh;
    Pixel **fb = state->framebuff;
    Transform_Vars *tf = params->tf;
    struct timespec start, end, frame_d;
    #ifdef PERFCOUNT
    Telem t = { .drawing = 0.f, .rendering = 0.f, .input = 0.f, .transform = 0.f, .it = 0};
    #endif
    Vec3 camera = {0.f, 0.f, 0.f, 0.f};
    size_t map_size = strlen(shader_map);

    while (running) {
        #ifdef PERFCOUNT
        clock_gettime(CLOCK_REALTIME, &start);
        frame_d.tv_nsec = start.tv_nsec;
        frame_d.tv_sec = start.tv_sec;
        #endif
        int termx, termy;
        get_term_size(params->term);
        termx = (int) params->term->x;
        termy = (int) params->term->y;
        // process user-input events
        int elmt = 0;

        while (Q_get(q, &elmt) > 0)
            process_input(tf, elmt);

        //tf->alpha += PI / 1000;
        //tf->beta += PI / 2000;
        //tf->gamma += PI / 3000;
        #ifdef PERFCOUNT
        clock_gettime(CLOCK_REALTIME, &end);
        t.input += delta_time(&start, &end);
        clock_gettime(CLOCK_REALTIME, &start);
        #endif
        mat3x3 tr_mat, ofst_mat, world_mat, scale_mat;
        mat4x4 proj_mat;
        get_proj_mat(proj_mat, termx, termy, params->theta, params->viewing_distance, params->focal_distance);
        get_tr_mat(tf->v, tr_mat);
        {
            Vec3 scale_v = { .x = (float)termx, .y = (float)termy, .z = 0.f};
            get_scale_mat(&scale_v, scale_mat);
        }
        {
            mat3x3 yaw_mat, pitch_mat, roll_mat, tmp_mat;
            get_yaw_mat(tf->alpha, yaw_mat);
            get_pitch_mat(tf->beta, pitch_mat);
            get_roll_mat(tf->gamma, roll_mat);
            mat3x3_mul(yaw_mat, pitch_mat, tmp_mat);
            mat3x3_mul(roll_mat, tmp_mat, world_mat);
        }
        #ifdef PERFCOUNT
        clock_gettime(CLOCK_REALTIME, &end);
        t.transform += delta_time(&start, &end);
        clock_gettime(CLOCK_REALTIME, &start);
        #endif

        // transformation pipeline

        for (int j = 0; j < mesh->s; j++) {
            Tri tri = *mesh[j].t;

            for (int i = 0; i < 3; i++) {
                Vec3 *v = &tri.v[i];
                // user input rotation
                mat3x3_Vec3_mul(world_mat, v, v);
                // user input translation
                mat4x3_Vec3_mul(tr_mat, v, v);
                // z-offset pour controler la distance écran - objet
                Vec3 v0 = { .x = 0.f, .y = 0.f, .z = params->translation_ofst};
                Vec3_add(v, &v0, v);
            }

            {
                Vec3 normal, line1, line2, vn1, vn2, vn3;
                float l;
                Vec3_line(&tri.v[0], &tri.v[1], &line1);
                Vec3_line(&tri.v[0], &tri.v[2], &line2);
                Vec3_cross(&line1, &line2, &normal);
                l = 1 / Vec3_norm(&normal);
                Vec3_scale(&normal, l, &normal);
                Vec3_line(&camera, &tri.v[0], &vn1);
                Vec3_line(&camera, &tri.v[1], &vn2);
                Vec3_line(&camera, &tri.v[2], &vn3);
                #ifndef WIREFRAME

                if (Vec3_dot(&vn1, &normal) >= 0.f && Vec3_dot(&vn2, &normal) >= 0.f && Vec3_dot(&vn3, &normal) >= 0.f)
                    continue;

                #endif
                {
                    // illumination
                    Vec3 light_direction;
                    Vec3_cpy(&camera, &light_direction);
                    light_direction.z = -1.f;
                    float ool = 1 / Vec3_norm(&light_direction);
                    Vec3_scale(&light_direction, ool, &light_direction);
                    float dp = Vec3_dot(&light_direction, &normal);
                    tri.s = (int)((map_size - 1) * dp);
                    // tri.c = COLOR_WHITE;
                }
            }

            // 2D projection
            Tri tp;
            memcpy(&tp, &tri, sizeof(Tri));

            for (int i = 0; i < 3; i++) {
                Vec3 *v = &tp.v[i];
                mat4x4_Vec3_mul(proj_mat, v, v);
                // offset des coordonées de [-1., 1.] vers [0., 1.]
                Vec3 v1 = { .x = 1.f, .y = 1.f, .z = 0.f };
                Vec3_add(v, &v1, v);
                Vec3_scale(v, 0.5f, v);
                // scaling des coordonées à la résolution du terminal
                mat3x3_Vec3_mul(scale_mat, v, v);
            }

            // clipping
            // {
            //     float q = params->viewing_distance / (params->viewing_distance - params->focal_distance);
            //     float zmax = -params->focal_distance * q;
            //     if (tri.v[0].z <= zmax || tri.v[1].z <= zmax || tri.v[2].z <= zmax)
            //         continue;
            // }
            {
                // rasterization
                #ifndef WIREFRAME
                fillTriangle(fb, zbuff, &tp);
                #else
                int shader = strlen(shader_map) - 1;
                drawline(fb, tp.v[0].x, tp.v[0].y, tp.v[1].x, tp.v[1].y, tp.c, shader);
                drawline(fb, tp.v[1].x, tp.v[1].y, tp.v[2].x, tp.v[2].y, tp.c, shader);
                drawline(fb, tp.v[2].x, tp.v[2].y, tp.v[1].x, tp.v[1].y, tp.c, shader);
                #endif
            }
        }

        #ifdef PERFCOUNT
        clock_gettime(CLOCK_REALTIME, &end);
        t.rendering += delta_time(&start, &end);
        clock_gettime(CLOCK_REALTIME, &start);
        #endif
        // draw framebuffer to screen
        draw(fb);
        #ifdef PERFCOUNT
        clock_gettime(CLOCK_REALTIME, &end);
        t.drawing += delta_time(&start, &end);
        clock_gettime(CLOCK_REALTIME, &end);
        float idle = FRAME_US - delta_time(&frame_d, &end) * 1000;
        t.fo = idle;
        drawdebug(&t);
        t.it++;
        //if (idle > 0)
        //    msleep((long)idle);
        #endif
    }

    return 0;
}

void sig_handler(int signum)
{
    running = 0;
}

int main()
{
    running = 1;
    signal(SIGINT, sig_handler);
    Vec2 term;
    get_term_size(&term);
    int termx = TERMX;
    int termy = TERMY;

    if (termx <= 0 || termy <= 0)
        return -1;

    // clear screen
    printf("\033[2J");
    // hide cursor
    printf("\e[?25l");
    // tty raw mode, non buffered io
    struct termios mode;
    tcgetattr(0, &mode);
    mode.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &mode);
    int *qp = malloc(sizeof(int) * QUEUE_SIZE);

    if (!qp)
        goto cleanup;

    Mesh *mesh = malloc(sizeof(Mesh) * SCENE_SIZE);

    if (!mesh)
        goto cleanup;

    memset(mesh, 0, sizeof(Mesh) * SCENE_SIZE);
    mesh->s = SCENE_SIZE;

    for (int i = 0; i < mesh->s; i++)
        mesh[i].t = &_scene[i];

    Pixel **framebuff = malloc(sizeof(Pixel *) * TERMY);

    if (!framebuff)
        goto cleanup;

    float **zbuff = malloc(sizeof(float *) * TERMY);

    if (!zbuff)
        goto cleanup;

    for (int j = 0; j < TERMY; j++) {
        Pixel *pfb = malloc(sizeof(Pixel) * TERMX);
        memset(pfb, 0, sizeof(Pixel) * TERMX);
        framebuff[j] = pfb;
        float *pzb = malloc(sizeof(float) * TERMX);
        memset(pzb, 0, sizeof(float) * TERMX);
        zbuff[j] = pzb;
    }

    // io thread init
    pthread_t _kb_input;
    Q q = { .head = 0, .tail = 0, .size = QUEUE_SIZE, .data = qp};
    pthread_create(&_kb_input, NULL, kb_input, &q);
    // hack pour forcer la rotation sur le centre de l'objet
    {
        Vec3 ofst = {.x = 0.f, .y = 0.f, .z = 0.f};
        mesh_center(mesh, &ofst);
        mat3x3 tr_mat;
        get_tr_mat(&ofst, tr_mat);

        for (int i = 0; i < mesh->s; i++) {
            for (int j = 0; j < 3; j++) {
                Vec3 *v = &(mesh[i].t->v[j]);
                mat4x3_Vec3_mul(tr_mat, v, v);
            }
        }
    }
    Vec3 ofst = {.x = 0.f, .y = 0.f, .z = 0.f};
    Game_State state = {
        .q = &q,
        .zbuff = zbuff,
        .framebuff = framebuff,
        .mesh = mesh,
    };
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
        .translation_ofst = 5.f,
        .viewing_distance = 1000.f,
        .theta = PI * 0.5f,
    };
    entrypoint_tri(&state, &params);
    goto cleanup;
cleanup:

    if (qp)
        free(qp);

    if (mesh)
        free(mesh);

    if (framebuff) {
        for (int i = 0; i < TERMY; i++) {
            if (framebuff[i] != NULL)
                free(framebuff[i]);
        }
    }

    if (zbuff) {
        for (int i = 0; i < TERMY; i++) {
            if (zbuff[i] != NULL)
                free(zbuff[i]);
        }
    }

    // show cursor
    printf("\e[?25h");
    return 0;
}
