#include "fifo.h"
#include "matrix.h"

#ifndef TYPES_H
#define TYPES_H

typedef enum { COLOR_NONE, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE, COLOR_TEMPLATE } Color;

const char *color_map[] = {
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
    float x;
    float y;
    float z;
    int shader;
    Color color;
} Pixel_A;

// typedef struct {
//     Vec3 v;
//     int shader;
//     Color color;
// } Pixel_A;

typedef struct {
    float alpha; // yaw
    float beta; // pitch
    float gamma; // roll
    float theta; // FOV
    float x_ofst;
    float y_ofst;
    float z_ofst;
} Transform_Vars;

typedef struct {
    Q *q;
    Pixel_A *p;
    Pixel_A *p1;
    Pixel **framebuff;
    float **zbuff;
} Game_State;

#endif
