#ifndef DRAW_H
#define DRAW_H

#include "types.h"
// static const char *shader_map = " .'`,^:\";~-_+<>i!lI?/\\|()1{}[]rcvunxzjftLCJUYXZO0Qoahkbdpqwm*WMB8&%$#@";
static const char *shader_map = " .,-~:;=!*#$@";
// static const char *shader_map = "@$#8&L%?=!;:~-,. ";
static void copyinto(char **buff, const char *str);
void draw(Pixel **);
void drawdebug(Telem *);
void drawfb(Pixel **fb, int x, int y, Color c, int shader);

#endif
