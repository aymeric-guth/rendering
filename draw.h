#ifndef DRAW_H
#define DRAW_H

#include "types.h"

static void copyinto(char **buff, const char *str);
void draw(Pixel **);
void drawdebug(Telem *);
void drawfb(Pixel **, int, int, Color, int);

#endif
