#include "rasterizer.h"
#include "constants.h"
#include "types.h"
#include "matrix.h"


// Horizontal line
void lcd_hline(Pixel **fb, int x1, int x2, int y)
{
    if (x1 >= x2)
        SWAP(x1, x2);

    for (; x1 <= x2; x1++)
        drawfb(fb, x1, x2, COLOR_WHITE, 12);
}

void drawline(Pixel **fb, int x1, int y1, int x2, int y2, Color c, int shader)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = abs(dx);
    dy1 = abs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;

    if (dy1 <= dx1) {
        if (dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        } else {
            x = x2;
            y = y2;
            xe = x1;
        }

        drawfb(fb, x, y, c, shader);

        for (i = 0; x < xe; i++) {
            x++;

            if (px < 0)
                px = px + 2 * dy1;
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    y++;
                else
                    y--;

                px = px + 2 * (dy1 - dx1);
            }

            drawfb(fb, x, y, c, shader);
        }

        return;
    }

    if (dy >= 0) {
        x = x1;
        y = y1;
        ye = y2;
    } else {
        x = x2;
        y = y2;
        ye = y1;
    }

    drawfb(fb, x, y, c, shader);

    for (i = 0; y < ye; i++) {
        y++;

        if (py <= 0)
            py = py + 2 * dx1;
        else {
            if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                x++;
            else
                x--;

            py = py + 2 * (dx1 - dy1);
        }

        drawfb(fb, x, y, c, shader);
    }
}

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

float crossProduct(Vec3 *v1, Vec3 *v2)
{
    return v1->x * v2->y - v1->y * v2->x;
}

// Fill a triangle - Bresenham method
// Original from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
// http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void fillTriangle(Pixel **fb, float **zbuff, Tri *tri)
{
    int x1, y1, x2, y2, x3, y3;
    float z1, z2, z3;
    x1 = tri->v[0].x;
    y1 = tri->v[0].y;
    z1 = tri->v[0].z;
    x2 = tri->v[1].x;
    y2 = tri->v[1].y;
    z2 = tri->v[0].z;
    x3 = tri->v[2].x;
    y3 = tri->v[2].y;
    z3 = tri->v[0].z;
    /* get the bounding box of the triangle */
    int maxX = MAX(x1, MAX(x2, x3));
    int minX = MIN(x1, MIN(x2, x3));
    int maxY = MAX(y1, MAX(y2, y3));
    int minY = MIN(y1, MIN(y2, y3));
    /* spanning vectors of edge (v1,v2) and (v1,v3) */
    Vec3 vs1 = {.x = x2 - x1, .y = y2 - y1, .z = 0};
    Vec3 vs2 = {.x = x3 - x1, .y = y3 - y1, .z = 0};

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            Vec3 q = {.x = x - x1, .y = y - y1};
            float s = crossProduct(&q, &vs2) / crossProduct(&vs1, &vs2);
            float t = crossProduct(&vs1, &q) / crossProduct(&vs1, &vs2);

            if ((s >= 0) && (t >= 0) && (s + t <= 1))
                drawfb(fb, x, y, tri->c, tri->s);
        }
    }
}
