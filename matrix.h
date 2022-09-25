#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec3;

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    int x;
    int y;
} Vec2_Int;

typedef struct {
    Vec2_Int *a;
    Vec2_Int *b;
} Line_Int;

typedef struct {
    Vec3 v[3];
    int c;
    int s;
} Tri;

typedef struct {
    Tri *t;
    void *c;
    size_t s;
} Mesh;

typedef float mat4x4[4][4];
typedef float mat4x3[4][3];
typedef float mat3x3[3][3];

float Vec2_cross(Vec2 *a, Vec2 *b);

void Vec3_add(Vec3 *, Vec3 *, Vec3 *);
void Vec3_mul(Vec3 *, Vec3 *, Vec3 *);
void Vec3_scale(Vec3 *, float, Vec3 *);
float Vec3_dot(Vec3 *, Vec3 *);
void Vec3_cross(Vec3 *, Vec3 *, Vec3 *);
float Vec3_norm(Vec3 *);
void Vec3_line(Vec3 *, Vec3 *, Vec3 *);
void Vec3_cpy(Vec3 *i, Vec3 *o);

void mat4x4_mul(mat4x4, mat4x4, mat4x4);

void mat4x4_Vec3_mul(mat4x4, Vec3 *, Vec3 *);
void mat3x3_Vec3_mul(mat3x3, Vec3 *, Vec3 *);
void mat4x3_Vec3_mul(mat4x3, Vec3 *, Vec3 *);

void get_yaw_mat(float, mat3x3);
void get_pitch_mat(float, mat3x3);
void get_roll_mat(float, mat3x3);
void get_tr_mat(Vec3 *, mat3x3);
void get_scale_mat(Vec3 *v, mat3x3 mat);
void get_proj_mat(mat4x4 mat, int termx, int termy, float theta, float viewing_distance, float focal_distance);

void mat3x3_mul(mat3x3, mat3x3, mat3x3);

#endif
