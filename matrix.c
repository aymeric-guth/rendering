#include <math.h>

#include "matrix.h"

void Vec3_add(Vec3 *i1, Vec3 *i2, Vec3 *o)
{
    o->x = i1->x + i2->x;
    o->y = i1->y + i2->y;
    o->z = i1->z + i2->z;
}

void Vec3_scale(Vec3 *i, float k, Vec3 *o)
{
    o->x = i->x * k;
    o->y = i->y * k;
    o->z = i->z * k;
}

void Vec3_cpy(Vec3 *i, Vec3 *o)
{
    o->x = i->x;
    o->y = i->y;
    o->z = i->z;
    o->w = i->w;
}

void mat4x4_Vec3_mul(mat4x4 mat, Vec3 *i, Vec3 *o)
{
    float x = i->x;
    float y = i->y;
    float z = i->z;
    o->x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + mat[3][0];
    o->y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + mat[3][1];
    o->z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + mat[3][2];
    float w = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + mat[3][3];

    if (w != 0.f) {
        float oow = 1.f / w;
        o->x = o->x * oow;
        o->y = o->y * oow;
        o->z = o->z * oow;
    }
}

void mat3x3_Vec3_mul(mat3x3 mat, Vec3 *i, Vec3 *o)
{
    float x = i->x;
    float y = i->y;
    float z = i->z;
    o->x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0];
    o->y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1];
    o->z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2];
}

void mat4x3_Vec3_mul(mat4x3 mat, Vec3 *i, Vec3 *o)
{
    float x = i->x;
    float y = i->y;
    float z = i->z;
    o->x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + 1.f * mat[3][0];
    o->y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + 1.f * mat[3][1];
    o->z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + 1.f * mat[3][2];
}

void get_yaw_mat(float alpha, mat3x3 mat)
{
    float cos_alpha = cosf(alpha);
    float sin_alpha = sinf(alpha);
    mat[0][0] = cos_alpha;
    mat[1][0] = sin_alpha;
    mat[2][0] = 0.f;
    mat[0][1] = -sin_alpha;
    mat[1][1] = cos_alpha;
    mat[2][1] = 0.f;
    mat[0][2] = 0.f;
    mat[1][2] = 0.f;
    mat[2][2] = 1.f;
}

void get_pitch_mat(float beta, mat3x3 mat)
{
    float cos_beta = cosf(beta);
    float sin_beta = sinf(beta);
    mat[0][0] = cos_beta;
    mat[1][0] = 0.f;
    mat[2][0] = -sin_beta;
    mat[0][1] = 0.f;
    mat[1][1] = 1.f;
    mat[2][1] = 0.f;
    mat[0][2] = sin_beta;
    mat[1][2] = 0.f;
    mat[2][2] = cos_beta;
}

void get_roll_mat(float gamma, mat3x3 mat)
{
    float cos_gamma = cosf(gamma);
    float sin_gamma = sinf(gamma);
    mat[0][0] = 1.f;
    mat[1][0] = 0.f;
    mat[2][0] = 0.f;
    mat[0][1] = 0.f;
    mat[1][1] = cos_gamma;
    mat[2][1] = sin_gamma;
    mat[0][2] = 0.f;
    mat[1][2] = -sin_gamma;
    mat[2][2] = cos_gamma;
}

void get_tr_mat(Vec3 *vec, mat4x3 mat)
{
    mat[0][0] = 1.f;
    mat[1][0] = 0.f;
    mat[2][0] = 0.f;
    mat[3][0] = vec->x;
    mat[0][1] = 0.f;
    mat[1][1] = 1.f;
    mat[2][1] = 0.f;
    mat[3][1] = vec->y;
    mat[0][2] = 0.f;
    mat[1][2] = 0.f;
    mat[2][2] = 1.f;
    mat[3][2] = vec->z;
}
void get_scale_mat(Vec3 *v, mat3x3 mat)
{
    mat[0][0] = v->x;
    mat[1][0] = 0.f;
    mat[2][0] = 0.f;
    mat[0][1] = 0.f;
    mat[1][1] = v->y;
    mat[2][1] = 0.f;
    mat[0][2] = 0.f;
    mat[1][2] = 0.f;
    mat[2][2] = v->z;
}

void mat3x3_mul(mat3x3 mata, mat3x3 matb, mat3x3 mat)
{
    mat[0][0] = mata[0][0] * matb[0][0] + mata[0][1] * matb[1][0] + mata[0][2] * matb[2][0];
    mat[0][1] = mata[0][0] * matb[0][1] + mata[0][1] * matb[1][1] + mata[0][2] * matb[2][1];
    mat[0][2] = mata[0][0] * matb[0][2] + mata[0][1] * matb[1][2] + mata[0][2] * matb[2][2];
    mat[1][0] = mata[1][0] * matb[0][0] + mata[1][1] * matb[1][0] + mata[1][2] * matb[2][0];
    mat[1][1] = mata[1][0] * matb[0][1] + mata[1][1] * matb[1][1] + mata[1][2] * matb[2][1];
    mat[1][2] = mata[1][0] * matb[0][2] + mata[1][1] * matb[1][2] + mata[1][2] * matb[2][2];
    mat[2][0] = mata[2][0] * matb[0][0] + mata[2][1] * matb[1][0] + mata[2][2] * matb[2][0];
    mat[2][1] = mata[2][0] * matb[0][1] + mata[2][1] * matb[1][1] + mata[2][2] * matb[2][1];
    mat[2][2] = mata[2][0] * matb[0][2] + mata[2][1] * matb[1][2] + mata[2][2] * matb[2][2];
}

float Vec3_dot(Vec3 *a, Vec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

float Vec2_cross(Vec2 *a, Vec2 *b)
{
    return a->x * b->y - a->y * b->x;
}

void Vec3_cross(Vec3 *a, Vec3 *b, Vec3 *o)
{
    o->x = a->y * b->z - a->z * b->y;
    o->y = a->z * b->x - a->x * b->z;
    o->z = a->x * b->y - a->y * b->x;
}

float Vec3_norm(Vec3 *v)
{
    float x = v->x;
    float y = v->y;
    float z = v->z;
    return sqrtf(x * x + y * y + z * z);
}

void Vec3_line(Vec3 *i1, Vec3 *i2, Vec3 *o)
{
    o->x = i2->x - i1->x;
    o->y = i2->y - i1->y;
    o->z = i2->z - i1->z;
}
