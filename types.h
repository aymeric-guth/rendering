#include "matrix.h"
#include "constants.h"
#include "fifo.h"

#ifndef TYPES_H
#define TYPES_H


static const char *color_map[] = {
    [COLOR_NONE] = "",
    [COLOR_BLACK] = "\x1b[30;1m",
    [COLOR_RED] = "\x1b[31;1m",
    [COLOR_GREEN] = "\x1b[32;1m",
    [COLOR_YELLOW] = "\x1b[33;1m",
    [COLOR_BLUE] = "\x1b[34;1m",
    [COLOR_MAGENTA] = "\x1b[35;1m",
    [COLOR_CYAN] = "\x1b[36;1m",
    [COLOR_WHITE] = "\x1b[37;1m",
    [COLOR_TEMPLATE] = "\x1b[38;5;$%dm"
};

typedef struct {
    int shader;
    Color color;
} Pixel;

typedef struct {
    float alpha; // yaw
    float beta; // pitch
    float gamma; // roll
    Vec3 *v;
} Transform_Vars;

typedef struct {
    Q *q;
    float **zbuff;
    Pixel **framebuff;
    Pixel **last;
    Mesh *mesh;
} Game_State;

typedef struct {
    Transform_Vars *tf;
    Vec2 *term;
    // znear = distance from camera to screen
    float focal_distance;
    // zfar = max rendering distance
    float viewing_distance;
    float translation_ofst;
    float theta; // FOV
} Render_Params;

#endif
