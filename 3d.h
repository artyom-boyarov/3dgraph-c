#ifndef H3D_H
#define H3D_H


#define WIDTH 256
#define HEIGHT 240

struct vec3d {
    float x,y,z;
};

struct triangle {
    struct vec3d p[3];
};

struct mesh {
    struct triangle* tris;
};

struct mat4x4 {
    float m[4][4];
};

void MultiplyMatrixVector(struct vec3d* i, struct vec3d* o, struct mat4x4* m);
void init();
int update(float felapsed);
void close();

#endif
