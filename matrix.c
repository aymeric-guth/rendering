#include "matrix.h"
#include "math.h"

void Vec3_add(Vec3 *i1, Vec3 *i2, Vec3 *o)
{
    o->x = i1->x + i2->x;
    o->y = i1->y + i2->y;
    o->z = i1->z + i2->z;
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
