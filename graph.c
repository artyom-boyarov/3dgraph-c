#include <SDL2/SDL.h>
#include <stdlib.h>

static SDL_Window* window;
static SDL_Renderer* renderer;

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

#define GET_COL(col)  (col >> 24), (col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF

int graph_init(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL");
		return -1;
	}

	if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window and renderer");
		return -2;
    }
    return 0;
}

uint32_t make_col(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((r << 24) | (g << 16) | (b << 8) | a);
}

void draw_line(int x1,int y1, int x2, int y2, uint32_t col) {
    SDL_SetRenderDrawColor(renderer, GET_COL(col));
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void draw_rect_nofill(int x1,int y1, int x2, int y2, uint32_t col) {
    SDL_Rect rect = {
        .x = x1,
        .y = y1,
        .w = (x2-x1),
        .h = (y2-y1)
    };

    SDL_SetRenderDrawColor(renderer, GET_COL(col));
    SDL_RenderDrawRect(renderer, &rect);
}

void clear(uint32_t col) {
    SDL_SetRenderDrawColor(renderer, GET_COL(col));
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void draw_tri_nofill(int x1,int y1, int x2, int y2, int x3, int y3, uint32_t col) {
    SDL_SetRenderDrawColor(renderer, GET_COL(col));
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

void draw_rect_fill(int x1,int y1, int x2, int y2, uint32_t col) {
    SDL_Rect rect = {
        .x = x1,
        .y = y1,
        .w = (x2-x1),
        .h = (y2-y1)
    };

    SDL_SetRenderDrawColor(renderer, GET_COL(col));
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
}

void fill_top_flat_tri(int baseX1, int baseY1, int baseX2, int baseY2, int bottomX, int bottomY) {

    int leftX = (baseX1 < baseX2) ? baseX1 : baseX2;
    int leftY = (baseX1 < baseX2) ? baseY1 : baseY2;
    int rightX = (baseX1 > baseX2) ? baseX1 : baseX2;
    int rightY = (baseX1 > baseX2) ? baseY1 : baseY2;
    float lProp = (float)(bottomX - leftX) / (float)(leftY - bottomY);
    float rProp = (float)(rightX - bottomX) / (float)(rightY - bottomY);

    for (int y = leftY; y <= bottomY; y++)
    {
        //Find x coordinates
        int lCoord = leftX + (lProp * (leftY - y));
        int rCoord = rightX - (rProp * (rightY - y));
        //printf("Drawing line from %d to %d\n", lCoord, rCoord);
        SDL_RenderDrawLine(renderer, lCoord, y, rCoord, y);
    }
}

void fill_bottom_flat_tri(int topX, int topY, int baseX1, int baseY1, int baseX2, int baseY2) {
    //Find corners
    int leftX = (baseX1 < baseX2) ? baseX1 : baseX2;
    int leftY = (baseX1 < baseX2) ? baseY1 : baseY2;
    int rightX = (baseX1 > baseX2) ? baseX1 : baseX2;
    int rightY = (baseX1 > baseX2) ? baseY1 : baseY2;
    //Find proportions
    float lProp = (float)(topX - leftX) / (float)(topY - leftY);
    float rProp = (float)(rightX - topX) / (float)(topY - rightY);

    for (int y = leftY; y >= topY; y--)
    {
        //Find x coordinates
        int lCoord = leftX + (lProp * (y - leftY));
        int rCoord = rightX - (rProp * (y - rightY));
        //printf("Drawing line from %d to %d\n", lCoord, rCoord);
        SDL_RenderDrawLine(renderer, lCoord, y, rCoord, y);
    }
    //Confirms
}



void draw_tri_fill(int x1,int y1, int x2, int y2, int x3, int y3, uint32_t col) {
/* Algorithm:
 * 1. Sort coordinates top to bottom
 * 2. Split into 2 triangles - top flat and bottom flat and draw
 */
    SDL_SetRenderDrawColor(renderer, GET_COL(col));

    // Sort vertices
    if (y1>y2) {swap(&y1, &y2); swap(&x1, &x2);}
    if (y1>y3) {swap(&y1, &y3); swap(&x1, &x3);}
    if (y2>y3) {swap(&y2, &y3); swap(&x2, &x3);}


    if (y1 == y2)
        fill_top_flat_tri(x1,y1,x2,y2,x3,y3);
    else if (y2 == y3)
        fill_bottom_flat_tri(x1,y1,x2,y2,x3,y3);
    else {
        int midX,midY;
        midY = y2;
        midX = (((float)(y2 - y1) / (float)(y3 - y1))*(float)(x3 - x1)) + x1;
        fill_bottom_flat_tri(x1,y1,x2,y2,midX,midY);
        fill_top_flat_tri(x2,y2,midX,midY,x3,y3);
    }
}



int graph_process_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return -1;
        }
    }
    return 0;
}

void graph_close() {
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void delay(int ms) {
    SDL_Delay(ms);
}

void commit() {
    SDL_RenderPresent(renderer);
}
