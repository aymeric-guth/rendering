#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CYCLES 1000
#define FRAME_US 16.666667f
#define PI 3.141592653589793f
#define QUEUE_SIZE 16
typedef enum {
    COLOR_NONE,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_TEMPLATE
} Color;
#define ROT_STEP PI / 75.f;
#define TR_STEP 0.1f;
#define PERFCOUNT
// #define WIREFRAME
#define HASCOLOR

#endif
