#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdint.h>
#include <stdbool.h>

#include "types.h"
#include "draw.h"

// Swap two bytes
#define SWAP(x,y) do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

void set_pixel(uint8_t, uint8_t);
void lcd_hline(uint8_t, uint8_t, uint8_t);
void drawline(Pixel **, int, int, int, int, Color, int);
void fillTriangleslope(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void fillTriangle(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

#endif
