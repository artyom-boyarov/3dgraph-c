#ifndef GRAPH_MATH_H
#define GRAPH_MATH_H

struct vec3d {
    float x,y,z,w;
};

struct triangle {
    struct vec3d p[3];
};

struct tri_draw {
    struct vec3d p[3];
    int col;
};

struct mesh {
    struct triangle* tris;
};

struct mat4x4 {
    float m[4][4];
};

struct vec3d vec3d_add(struct vec3d a, struct vec3d b);
struct vec3d vec3d_sub(struct vec3d a, struct vec3d b);
struct vec3d vec3d_mul(struct vec3d a, int k);
struct vec3d vec3d_div(struct vec3d a, int k);
float vec3d_dotproduct(struct vec3d a, struct vec3d b);
float vec3d_length(struct vec3d v);
void vec3d_normalise(struct vec3d* v);
struct vec3d vec3d_crossproduct(struct vec3d a, struct vec3d b);
struct vec3d mat4x4_mul_vec3d(struct vec3d* i, struct mat4x4* m);
struct mat4x4 mat4x4_makeidentity();
struct mat4x4 mat4x4_makerotX(float fAngleRad);
struct mat4x4 mat4x4_makerotY(float fAngleRad);
struct mat4x4 mat4x4_makerotZ(float fAngleRad);
struct mat4x4 mat4x4_maketranslation(float x, float y, float z);
struct mat4x4 mat4x4_makeprojection(float fFovDeg, float fAspectRatio, float fNear, float fFar);
struct mat4x4 mat4x4_mul(struct mat4x4 m1, struct mat4x4 m2);

#endif
