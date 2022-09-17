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
    float z;
    float w;
} Vec4;

typedef struct {
    Vec3 v[3];
} Tri;

typedef float mat3x2[3][2];
void mat3x3(Vec3 *, Vec3 *, float [3][3]);
void mat4x4(Vec4 *, Vec4 *, float [4][4]);

#endif
