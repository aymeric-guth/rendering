#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdint.h>
#include <stdbool.h>

#include "types.h"
#include "draw.h"

// Swap two bytes
#define SWAP(x,y) do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

void set_pixel(uint8_t, uint8_t);
void drawline(Pixel **, int, int, int, int, Color, int);
void fillTriangleslope(Pixel **fb, int x0, int y0, int x1, int y1, int x2, int y2, int color);
void lcd_hline(Pixel **fb, int x1, int x2, int y);
void fillTriangle(Pixel **fb, float **zbuff, Tri *tri);

#endif
