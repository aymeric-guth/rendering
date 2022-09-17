#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

typedef struct {
    Vec3 v[3];
} Tri;

typedef float mat4x4[4][4];
typedef float mat4x3[4][3];
typedef float mat3x3[3][3];

void Vec3_add(Vec3 *, Vec3 *, Vec3 *);
void mat4x4_Vec3_mul(mat4x4, Vec3 *, Vec3 *);
void mat3x3_Vec3_mul(mat3x3, Vec3 *, Vec3 *);
void mat4x3_Vec3_mul(mat4x3, Vec3 *, Vec3 *);

void get_yaw_mat(float, mat3x3);
void get_pitch_mat(float, mat3x3);
void get_roll_mat(float, mat3x3);
void get_tr_mat(Vec3 *, mat3x3);

#endif
