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

void get_proj_mat(mat4x4 mat, int termx, int termy, float theta, float viewing_distance, float focal_distance)
{
    // rectilinear projection
    float a = (float)termy / (float)termx;
    float fov = 1 / tanf(theta * 0.5f);
    float q = viewing_distance / (viewing_distance - focal_distance);
    mat[0][0] = a * fov;
    mat[1][0] = 0.f;
    mat[2][0] = 0.f;
    mat[3][0] = 0.f;
    mat[0][1] = 0.f;
    mat[1][1] = fov;
    mat[2][1] = 0.f;
    mat[3][1] = 0.f;
    mat[0][2] = 0.f;
    mat[1][2] = 0.f;
    mat[2][2] = q;
    mat[3][2] = -focal_distance * q;
    mat[0][3] = 0.f;
    mat[1][3] = 0.f;
    mat[2][3] = 1.f;
    mat[3][3] = 0.f;
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

void Quat_conjugate(Quat *q1, Quat *q2)
{
    q2->w = q1->w;
    q2->x = -1 * q1->x;
    q2->y = -1 * q1->y;
    q2->z = -1 * q1->z;
}

void Quat_Vec3_mul(Quat *q, Vec3 *i, Vec3 *o)
{
    Quat qv = { .w = 0.f, .x = i->x, .y = i->y, .z = i->z};
    Quat qv1, qv2, qv3;
    Quat_mul(q, &qv, &qv1);
    Quat_conjugate(q, &qv2);
    Quat_mul(&qv1, &qv2, &qv3);
    o->w = qv3.w;
    o->x = qv3.x;
    o->y = qv3.y;
    o->z = qv3.z;
}

void Quat_mul(Quat *q1, Quat *q2, Quat *o)
{
    o->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
    o->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
    o->y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
    o->z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
}

void euler_to_Quat(float phi, float theta, float psi, Quat *q)
{
    q->w = cosf(phi / 2) * cosf(theta / 2) * cosf(psi / 2) + sinf(phi / 2) * sinf(theta / 2) * sinf(psi / 2);
    q->x = sinf(phi / 2) * cosf(theta / 2) * cosf(psi / 2) - cosf(phi / 2) * sinf(theta / 2) * sinf(psi / 2);
    q->y = cosf(phi / 2) * sinf(theta / 2) * cosf(psi / 2) + sinf(phi / 2) * cosf(theta / 2) * sinf(psi / 2);
    q->z = cosf(phi / 2) * cosf(theta / 2) * sinf(psi / 2) - sinf(phi / 2) * sinf(theta / 2) * cosf(psi / 2);
}


void Quat_to_euler(Quat *q, Vec3 *o)
{
    float t0 = 2. * (q->w * q->x + q->y * q->z);
    float t1 = 1. - 2. * (q->x * q->x + q->y * q->y);
    o->x = atan2(t0, t1);
    float t2 = 2. * (q->w * q->y - q->z * q->x);
    t2 = t2 > 1. ? 1. : t2;
    t2 = t2 < -1. ? -1. : t2;
    o->y = asin(t2);
    float t3 = 2 * (q->w * q->z + q->x * q->y);
    float t4 = 1. - 2. * (q->y * q->y + q->z * q->z);
    o->z = atan2(t3, t4);
}
