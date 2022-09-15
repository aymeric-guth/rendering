#define SIZE3D 33
#define SPACE SIZE3D * SIZE3D * SIZE3D
#define SCREEN_WIDTH 124 / 2
#define SCREEN_HEIGHT 33
#define DEBUG 0
#define CYCLES 1000
#define FRAME 24
#define PI 3.141592653589793f
#define QUEUE_SIZE 16

typedef enum { NONE, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, TEMPLATE } Color;

typedef struct {
    // float x;
    // float y;
    // float z;
    int shader;
    Color color;
} Pixel;

// typedef struct {
//     float x;
//     float y;
//     float z;
// } Vector3;
//
// typedef struct {
//     float x;
//     float y;
// } Vector2;

typedef struct {
    float x;
    float y;
    float z;
    int shader;
    Color color;
} Pixel_A;

const char *color_map[] = {
    [NONE] = "",
    [BLACK] = "\x1b[30;1m",
    [RED] = "\x1b[31;1m",
    [GREEN] = "\x1b[32;1m",
    [YELLOW] = "\x1b[33;1m",
    [BLUE] = "\x1b[34;1m",
    [MAGENTA] = "\x1b[35;1m",
    [CYAN] = "\x1b[36;1m",
    [WHITE] = "\x1b[37;1m",
    [TEMPLATE] = "\x1b[38;5;$%dm"
};


typedef enum { KEY_A, KEY_W, KEY_S, KEY_D } Input;
