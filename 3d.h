#ifndef H3D_H
#define H3D_H

#include "graph_math.h"

#define WIDTH 256
#define HEIGHT 240


int LoadFromObjectFile(const char* fname, struct triangle* out_tris);
float DotProduct(struct vec3d* a, struct vec3d* b);
void CrossProduct(struct vec3d* o, struct vec3d* a, struct vec3d* b);
void init();
int update(float felapsed);
void close();

#endif
