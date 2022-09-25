#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "draw.h"

extern int TERMX;
extern int TERMY;

static void copyinto(char **buff, const char *str)
{
    strcpy(*buff, str);
    *buff += strlen(str);
}

void draw(Pixel **framebuff)
{
    const size_t BUFFSIZE = TERMX * 10;
    Pixel *px;
    char buff[BUFFSIZE];

    for (int j = 0; j < TERMY; j++) {
        memset(buff, 0, BUFFSIZE);
        char *c = buff;

        for (int i = 0; i < TERMX; i++) {
            px = &framebuff[j][i];

            if (px->shader > 0 && px->color != COLOR_NONE) {
                #ifdef HASCOLOR
                copyinto(&c, color_map[px->color]);
                #endif
                const char *sym = &shader_map[px->shader];
                const char s[3] = {*sym, *sym, 0};
                copyinto(&c, s);
            } else
                copyinto(&c, "  ");

            px->shader = 0;
            px->color = COLOR_NONE;
        }

        // move cursor to line j + end color
        #ifdef HASCOLOR
        printf("\033[%d;0H%s\x1b[0m\n", j, buff);
        #else
        printf("\033[%d;0H%s\n", j, buff);
        #endif
        //msleep(10);
    }
}

void drawdebug(Telem *t)
{
    printf("\033[%d;%dH", TERMY - 1, (TERMX - 1) / 2);
    int it = t->it;
    double fps = 1 / ((t->input + t->transform + t->rendering + t->drawing) / (double)it);
    printf("duration=%f, resolution=%dx%d, FPS=%lf, IO=%lf, Transform=%lf, Rendering=%lf, Drawing=%lf", t->fo, TERMX, TERMY, fps, t->input / it, t->transform / it, t->rendering / it, t->drawing / it);
}

void drawfb(Pixel **fb, int x, int y, Color c, int shader)
{
    if (x < TERMX && x >= 0 && y < TERMY && y >= 0) {
        fb[y][x].shader = shader;
        fb[y][x].color = c;
    }
}
