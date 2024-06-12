#include "3d.h"
#include <stdio.h>
#include "graph.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_TRIS 200
#define MAX_VERTS 600

#define LINE printf("%d\n", __LINE__)

static struct mat4x4 matProj;


static struct triangle tris[MAX_TRIS]; /*= {
    {{{0.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {1.0f,1.0f,0.0f}}},
    {{{0.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f}, {1.0f,0.0f,0.0f}}},
    {{{1.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f}, {1.0f,1.0f,1.0f}}},
    {{{1.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}, {1.0f,0.0f,1.0f}}},
    {{{1.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}, {0.0f,1.0f,1.0f}}},
    {{{1.0f,0.0f,1.0f}, {0.0f,1.0f,1.0f}, {0.0f,0.0f,1.0f}}},
    {{{0.0f,0.0f,1.0f}, {0.0f,1.0f,1.0f}, {0.0f,1.0f,0.0f}}},
    {{{0.0f,0.0f,1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}}},
    {{{0.0f,1.0f,0.0f}, {0.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}}},
    {{{0.0f,1.0f,0.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,0.0f}}},
    {{{1.0f,0.0f,1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f}}},
    {{{1.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}}},
}*/


static int n_tris = 1;
static float fTheta;
static struct vec3d vCamera;

int LoadFromObjectFile(const char* fname, struct triangle* out_tris) {
    FILE* file = fopen(fname, "r");
    if (!file)
        return -1;

    char* line;
    ssize_t read;
    size_t len = 0;
    struct vec3d vertices[MAX_VERTS];
    int c_v = 0, c_t = 0;
    while ((read = getline(&line, &len, file)) != -1) {

        if (line[0] == 'v' && line[1] != 'n') {
            // Vertex
            float x,y,z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            //printf("Read vertex %d: %f,%f,%f\n", c_v, x,y,z);
            vertices[c_v++] = (struct vec3d){x,y,z};
        }

        else if (line[0] == 'f') {
            int f1,f2,f3;
            sscanf(line, "f %d %d %d", &f1, &f2, &f3);

            //printf("Read face %d: %d,%d,%d\n", c_t, f1,f2,f3);
            out_tris[c_t++] = (struct triangle){{vertices[f1-1], vertices[f2-1], vertices[f3-1]}};
        }
    }

    if (line)
        free(line);
    close(file);
    return c_t;
}

void init() {
    if ((n_tris = LoadFromObjectFile("VideoShip.obj", tris)) < 0) {
        printf("Error loading triangles\n");
    }
    printf("Loaded %d triangles\n", n_tris);

    matProj = mat4x4_makeprojection(90.0f, (float)HEIGHT/(float)WIDTH, 0.1f, 1000.0f);
}

int compare_vec_z(const void* t1v, const void* t2v) {
    struct tri_draw* t1 = (struct tri_draw*) t1v;
    struct tri_draw* t2 = (struct tri_draw*) t2v;
    float z1 = (t1->p[0].z + t1->p[1].z + t1->p[2].z) / 3.0f;
    float z2 = (t2->p[0].z + t2->p[1].z + t2->p[2].z) / 3.0f;
    return z1 < z2;
}

int update(float felapsed) {

    struct mat4x4 matRotZ, matRotX;
    fTheta += 1.0f * felapsed;
    matRotZ = mat4x4_makerotZ(fTheta * 0.5f);
    matRotX = mat4x4_makerotX(fTheta);

    struct mat4x4 matTrans, matWorld;
    matTrans = mat4x4_maketranslation(0.0f, 0.0f, 16.0f);
    matWorld = mat4x4_makeidentity();
    matWorld = mat4x4_mul(matRotZ, matRotX);
    matWorld = mat4x4_mul(matWorld, matTrans);
    struct tri_draw tris_to_raster[MAX_TRIS];
    int c_t = 0;

    for (int c = 0; c < n_tris; c++) {
        struct triangle triProjected, triTransformed;
        struct triangle tri = tris[c];

        triTransformed.p[0] = mat4x4_mul_vec3d(&tri.p[0], &matWorld);
        triTransformed.p[1] = mat4x4_mul_vec3d(&tri.p[1], &matWorld);
        triTransformed.p[2] = mat4x4_mul_vec3d(&tri.p[2], &matWorld);

        struct vec3d normal, line1, line2;
        line1 = vec3d_sub(triTransformed.p[1], triTransformed.p[0]);
        line2 = vec3d_sub(triTransformed.p[2], triTransformed.p[0]);


        normal = vec3d_crossproduct(line1, line2);

        vec3d_normalise(&normal);
        struct vec3d vCameraRay = vec3d_sub(triTransformed.p[0], vCamera);
        if (vec3d_dotproduct(normal, vCameraRay) < 0.0f) {

            struct vec3d light_direction = {0.0f, 1.0f, -1.0f};
            vec3d_normalise(&light_direction);
            float abs_dp_c = vec3d_dotproduct(light_direction,normal);
            float dp =abs_dp_c;
            float col = (float)255*dp;

            // Project into 2d
            triProjected.p[0] = mat4x4_mul_vec3d(&triTransformed.p[0], &matProj);
            triProjected.p[1] = mat4x4_mul_vec3d(&triTransformed.p[1], &matProj);
            triProjected.p[2] = mat4x4_mul_vec3d(&triTransformed.p[2], &matProj);
            triProjected.p[0].w = 1.0f; triProjected.p[1].w = 1.0f; triProjected.p[2].w = 1.0f;

            triProjected.p[0] = vec3d_div(triProjected.p[0], triProjected.p[0].w);
            triProjected.p[1] = vec3d_div(triProjected.p[1], triProjected.p[1].w);
            triProjected.p[2] = vec3d_div(triProjected.p[2], triProjected.p[2].w);

            // Scale into view
            struct vec3d vOffsetView = (struct vec3d){1.0f,1.0f,0};
            triProjected.p[0] = vec3d_add(triProjected.p[0], vOffsetView);
            triProjected.p[1] = vec3d_add(triProjected.p[1], vOffsetView);
            triProjected.p[2] = vec3d_add(triProjected.p[2], vOffsetView);

            triProjected.p[0].x *= (0.5f * (float)WIDTH);
            triProjected.p[0].y *= (0.5f * (float)HEIGHT);
            triProjected.p[1].x *= (0.5f * (float)WIDTH);
            triProjected.p[1].y *= (0.5f * (float)HEIGHT);
            triProjected.p[2].x *= (0.5f * (float)WIDTH);
            triProjected.p[2].y *= (0.5f * (float)HEIGHT);

            tris_to_raster[c_t].p[0] = triProjected.p[0];
            tris_to_raster[c_t].p[1] = triProjected.p[1];
            tris_to_raster[c_t].p[2] = triProjected.p[2];
            tris_to_raster[c_t].col = col;
            c_t++;
        }
    }

    // Sort triangles by z  -take averages of z components and compare
    qsort(tris_to_raster, c_t, sizeof(struct tri_draw), compare_vec_z);

    clear(make_col(0,0,0,255));
    for (int i = 0; i < c_t; i++) {
        int col = abs(tris_to_raster[i].col);
        draw_tri_fill(tris_to_raster[i].p[0].x, tris_to_raster[i].p[0].y,
                          tris_to_raster[i].p[1].x, tris_to_raster[i].p[1].y,
                          tris_to_raster[i].p[2].x, tris_to_raster[i].p[2].y,
                          make_col(col,col,col,255));
    }
    commit();
    return 1;
}

void close() {
    //free(tris);
}
