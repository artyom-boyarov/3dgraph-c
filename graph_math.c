#include "graph_math.h"
#include <math.h>
#include <string.h>

struct vec3d vec3d_add(struct vec3d a, struct vec3d b) {
    return (struct vec3d){a.x+b.x, a.y+b.y,a.z+b.z, 1.0f};
}

struct vec3d vec3d_sub(struct vec3d a, struct vec3d b) {
    return (struct vec3d){a.x-b.x, a.y-b.y,a.z-b.z, 1.0f};
}

struct vec3d vec3d_mul(struct vec3d a, int k) {
    return (struct vec3d){a.x*k, a.y*k, a.z*k, 1.0f};
}

struct vec3d vec3d_div(struct vec3d a, int k) {
    return (struct vec3d){a.x/k, a.y/k, a.z/k, 1.0f};
}

float vec3d_dotproduct(struct vec3d a, struct vec3d b) {
    return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
}


float vec3d_length(struct vec3d v) {
    return sqrtf(vec3d_dotproduct(v,v));
}
void vec3d_normalise(struct vec3d* v) {
    float l = vec3d_length(*v);
    if (l != 0){
        v->x /= l;
        v->y /= l;
        v->z /= l;
    }
}

struct vec3d vec3d_crossproduct(struct vec3d a, struct vec3d b) {
    struct vec3d o;
    o.x = (a.y * b.z) - (a.z * b.y);
    o.y = (a.z * b.x) - (a.x * b.z);
    o.z = (a.x * b.y) - (a.y * b.x);
    return o;
}

struct vec3d mat4x4_mul_vec3d(struct vec3d* i, struct mat4x4* m) {
    struct vec3d o;
    o.x = (i->x * m->m[0][0]) + (i->y * m->m[1][0]) + (i->z * m->m[2][0]) + (i->w * m->m[3][0]);
    o.y = (i->x * m->m[0][1]) + (i->y * m->m[1][1]) + (i->z * m->m[2][1]) + (i->w * m->m[3][1]);
    o.z = (i->x * m->m[0][2]) + (i->y * m->m[1][2]) + (i->z * m->m[2][2]) + (i->w * m->m[3][2]);
    o.w = (i->x * m->m[0][3]) + (i->y * m->m[1][3]) + (i->z * m->m[2][3]) + (i->w * m->m[3][3]);
    return o;
}

struct mat4x4 mat4x4_makeidentity() {
    struct mat4x4 mat;
    memset(&mat, 0x0, sizeof(struct mat4x4));
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] =1.0f;
    mat.m[3][3] = 1.0f;
    return mat;
}


struct mat4x4 mat4x4_makerotX(float fAngleRad) {
    struct mat4x4 matrix;
    memset(&matrix, 0x0, sizeof(struct mat4x4));
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

struct mat4x4 mat4x4_makerotY(float fAngleRad) {
    struct mat4x4 matrix;
    memset(&matrix, 0x0, sizeof(struct mat4x4));
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

struct mat4x4 mat4x4_makerotZ(float fAngleRad) {
    struct mat4x4 matrix;
    memset(&matrix, 0x0, sizeof(struct mat4x4));
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

struct mat4x4 mat4x4_maketranslation(float x, float y, float z) {
    struct mat4x4 matrix;
    memset(&matrix, 0x0, sizeof(struct mat4x4));
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

struct mat4x4 mat4x4_makeprojection(float fFovDeg, float fAspectRatio, float fNear, float fFar) {
    float fFovRad = 1.0f / tanf((fFovDeg * 0.5f) / (180.0f * 3.14159f));
    struct mat4x4 matrix;
    memset(&matrix, 0x0, sizeof(struct mat4x4));
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

struct mat4x4 mat4x4_mul(struct mat4x4 m1, struct mat4x4 m2) {
    struct mat4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = (m1.m[r][0] * m2.m[0][c]) + (m1.m[r][1] * m2.m[1][c]) + (m1.m[r][2] * m2.m[2][c]) + (m1.m[r][3] * m2.m[3][c]);
    return matrix;
}










