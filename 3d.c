#include "3d.h"
#include <stdio.h>
#include "graph.h"

void init() {
}

int update(float felapsed) {
    clear(make_col(0,0,0,255));
    draw_line(1,1,100,100, make_col(255,0,0,255));
    printf("Elapsed: %f\n", felapsed);
    return 1;
}
