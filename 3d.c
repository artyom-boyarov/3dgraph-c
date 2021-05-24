#include "3d.h"
#include <stdio.h>
#include "graph.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static struct mat4x4 matProj;

static struct triangle tris[] = {
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
};


static int n_tris = 12;
static float fTheta;

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

    for (int c = 0; c < n_tris; c++) {
        struct triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;
        struct triangle tri = tris[c];

        MultiplyMatrixVector(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        MultiplyMatrixVector(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        MultiplyMatrixVector(&tri.p[2], &triRotatedZ.p[2], &matRotZ);

        MultiplyMatrixVector(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        MultiplyMatrixVector(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        MultiplyMatrixVector(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

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
        draw_tri_nofill(triProjected.p[0].x, triProjected.p[0].y,
                        triProjected.p[1].x, triProjected.p[1].y,
                        triProjected.p[2].x, triProjected.p[2].y,
                        make_col(255,255,255,255));

    }

    return 1;
}

void close() {
}
