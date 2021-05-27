#include "3d.h"
#include <stdio.h>
#include "graph.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_TRIS 200
#define MAX_VERTS 600

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
}*/;


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

float DotProduct(struct vec3d* a, struct vec3d* b) {
    float d = (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
    return d;
}

void CrossProduct(struct vec3d* o, struct vec3d* a, struct vec3d* b) {
    o->x = (a->y * b->z) - (a->z * b->y);
    o->y = (a->z * b->x) - (a->x * b->z);
    o->z = (a->x * b->y) - (a->y * b->x);
}

void MultiplyMatrixVector(struct vec3d* i, struct vec3d* o, struct mat4x4* m) {
    o->x = (i->x * m->m[0][0]) + (i->y * m->m[1][0]) + (i->z * m->m[2][0]) + m->m[3][0];
    o->y = (i->x * m->m[0][1]) + (i->y * m->m[1][1]) + (i->z * m->m[2][1]) + m->m[3][1];
    o->z = (i->x * m->m[0][2]) + (i->y * m->m[1][2]) + (i->z * m->m[2][2]) + m->m[3][2];
    float w = (i->x * m->m[0][3]) + (i->y * m->m[1][3]) + (i->z * m->m[2][3]) + m->m[3][3];

    if (w != 0.0f) {
        o->x /= w;
        o->y /= w;
        o->z /= w;
    }
}

void init() {
    //tris = malloc(n_tris* sizeof(struct triangle));
    if ((n_tris = LoadFromObjectFile("VideoShip.obj", tris)) < 0) {
        printf("Error loading triangles\n");
    }
    printf("Loaded %d triangles\n", n_tris);

    /*for (int i = 0; i < tris_c; i++) {
        printf("Triangle %d: %f,%f,%f %f,%f,%f %f,%f,%f\n", i, tris[i].p[0].x,
                                                             tris[i].p[0].y,
                                                             tris[i].p[0].z,
                                                             tris[i].p[1].x,
                                                             tris[i].p[1].y,
                                                             tris[i].p[1].z,
                                                             tris[i].p[2].x,
                                                             tris[i].p[2].y,
                                                             tris[i].p[2].z);
    }*/

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)WIDTH / (float)HEIGHT;
    float fFovRad = 1.0f / tanf((fFov * 0.5f )/ (180.0f / 3.14159f) );

    memset(&matProj, 0x0, sizeof matProj);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    // 30.44
}

int compare_vec_z(const void* t1v, const void* t2v) {
    struct tri_draw* t1 = (struct tri_draw*) t1v;
    struct tri_draw* t2 = (struct tri_draw*) t2v;
    float z1 = (t1->p[0].z + t1->p[1].z + t1->p[2].z) / 3.0f;
    float z2 = (t2->p[0].z + t2->p[1].z + t2->p[2].z) / 3.0f;
    return z1 < z2;
}

int update(float felapsed) {
    clear(make_col(0,0,0,255));

    struct mat4x4 matRotZ, matRotX;
    fTheta += 10.0f * felapsed;
    memset(&matRotZ, 0x0, sizeof matRotZ);

    matRotZ.m[0][0] = cosf(fTheta);
    matRotZ.m[0][1] = sinf(fTheta);
    matRotZ.m[1][0] = -sinf(fTheta);
    matRotZ.m[1][1] = cosf(fTheta);
    matRotZ.m[2][2] = 1.0f;
    matRotZ.m[3][3] = 1.0f;
    memset(&matRotX, 0x0, sizeof matRotX);

    matRotX.m[0][0] = 1.0f;
    matRotX.m[1][1] = cosf(fTheta * 0.5f);
    matRotX.m[1][2] = sinf(fTheta * 0.5f);
    matRotX.m[2][1] = -sinf(fTheta * 0.5f);
    matRotX.m[2][2] = cosf(fTheta * 0.5f);
    matRotX.m[3][3] = 1.0f;

    struct tri_draw tris_to_raster[MAX_TRIS];
    int c_t = 0;

    for (int c = 0; c < n_tris; c++) {
        struct triangle triTranslated, triProjected, triRotatedZ, triRotatedZX;
        struct triangle tri = tris[c];

        MultiplyMatrixVector(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        MultiplyMatrixVector(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        MultiplyMatrixVector(&tri.p[2], &triRotatedZ.p[2], &matRotZ);

        MultiplyMatrixVector(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        MultiplyMatrixVector(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        MultiplyMatrixVector(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 8.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 8.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 8.0f;

        struct vec3d normal, line1, line2;
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

        CrossProduct(&normal, &line1, &line2);

        float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        normal.x /= l; normal.y /= l; normal.z /= l;

        //if (normal.z < 0){
        if ((normal.x * (triTranslated.p[0].x - vCamera.x) +
            normal.y * (triTranslated.p[0].y - vCamera.y) +
            normal.z * (triTranslated.p[0].z - vCamera.z)) < 0.0f) {

            struct vec3d light_direction = {0.0f, 0.0f, -1.0f};
            float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
            light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;
            float dp = DotProduct(&normal,&light_direction);
            //printf("%f\n", dp);
            float col = (float)255*dp;

            MultiplyMatrixVector(&triTranslated.p[0], &triProjected.p[0], &matProj);
            MultiplyMatrixVector(&triTranslated.p[1], &triProjected.p[1], &matProj);
            MultiplyMatrixVector(&triTranslated.p[2], &triProjected.p[2], &matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= (0.5f * (float)WIDTH);
            triProjected.p[0].y *= (0.5f * (float)HEIGHT);
            triProjected.p[1].x *= (0.5f * (float)WIDTH);
            triProjected.p[1].y *= (0.5f * (float)HEIGHT);
            triProjected.p[2].x *= (0.5f * (float)WIDTH);
            triProjected.p[2].y *= (0.5f * (float)HEIGHT);

            /*printf("Storing triangle %d\n", c_t);
            printf("Triangle: %f,%f %f,%f, %f,%f\n", triProjected.p[0].x, triProjected.p[0].y,
                           triProjected.p[1].x, triProjected.p[1].y,
                          triProjected.p[2].x, triProjected.p[2].y);
            *///struct triangle* addr = &triProjected;
            //addr++;
            //memcpy(addr, &triProjected, sizeof(struct triangle));
            tris_to_raster[c_t].p[0] = triProjected.p[0];
            tris_to_raster[c_t].p[1] = triProjected.p[1];
            tris_to_raster[c_t].p[2] = triProjected.p[2];
            tris_to_raster[c_t].col = col;
            c_t++;

            /*draw_tri_fill(triProjected.p[0].x, triProjected.p[0].y,
                           triProjected.p[1].x, triProjected.p[1].y,
                          triProjected.p[2].x, triProjected.p[2].y,
                           make_col(col,col,col,255));*/
            //draw_tri_fill(68,59,146,65,145,127, make_col(0,0,255,255));

        }
    }


    // Sort triangles by z  -take averages of z components and compare
    qsort(tris_to_raster, c_t, sizeof(struct tri_draw), compare_vec_z);


     for (int i = 0; i < c_t; i++) {
//         printf("Drawing triangle %d\n", i);
         int col = abs(tris_to_raster[i].col);
//             printf("Triangle: %f,%f %f,%f, %f,%f colour: %d\n", tris_to_raster[c_t].p[0].x, tris_to_raster[c_t].p[0].y,
//                            tris_to_raster[c_t].p[1].x, tris_to_raster[c_t].p[1].y,
//                           tris_to_raster[c_t].p[2].x, tris_to_raster[c_t].p[2].y, col);
//
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
