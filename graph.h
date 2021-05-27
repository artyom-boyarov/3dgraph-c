#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>


#define OPAQUE 255

uint32_t make_col(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int graph_init(int width, int height);
void draw_line(int x1,int y1, int x2, int y2, uint32_t col);
void draw_rect_nofill(int x1,int y1, int x2, int y2, uint32_t col);
void draw_tri_nofill(int x1,int y1, int x2, int y2, int x3, int y3, uint32_t col);
void draw_rect_fill(int x1,int y1, int x2, int y2, uint32_t col);
void draw_tri_fill(int x1,int y1, int x2, int y2, int x3, int y3, uint32_t col);
void clear(uint32_t col);
int graph_process_events();
void graph_close();
void delay(int ms);
void commit();

#endif
