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

int LoadFromObjectFile(const char* fname, struct triangle* out_tris);
float DotProduct(struct vec3d* a, struct vec3d* b);
void CrossProduct(struct vec3d* o, struct vec3d* a, struct vec3d* b);
void MultiplyMatrixVector(struct vec3d* i, struct vec3d* o, struct mat4x4* m);
void init();
int update(float felapsed);
void close();

#endif
