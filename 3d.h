#ifndef H3D_H
#define H3D_H

struct vec3d {
    float x,y,z;
};

struct triangle {
    vec3d p[3];
};

struct mesh {
    struct triangle* tris;
}

void init();
int update(float felapsed);

#endif
