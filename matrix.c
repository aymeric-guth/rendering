#include "matrix.h"

void mat3x3(Vec3 *i, Vec3 *o, float mat[3][3])
{
    float x = i->x;
    float y = i->y;
    float z = i->z;
    o->x = mat[0][0] * x + mat[1][0] * y + mat[2][0] * z;
    o->y = mat[0][1] * x + mat[1][1] * y + mat[2][1] * z;
    o->z = mat[0][2] * x + mat[1][2] * y + mat[2][2] * z;
}

void mat4x4(Vec4 *i, Vec4 *o, float mat[4][4])
{
    float x = i->x;
    float y = i->y;
    float z = i->z;
    float w = i->w;
    o->x = mat[0][0] * x + mat[1][0] * y + mat[2][0] * z + mat[3][0] * w;
    o->y = mat[0][1] * x + mat[1][1] * y + mat[2][1] * z + mat[3][0] * w;
    o->z = mat[0][2] * x + mat[1][2] * y + mat[2][2] * z + mat[3][0] * w;
    o->w = mat[0][3] * x + mat[1][3] * y + mat[2][3] * z + mat[3][3] * w;
}
