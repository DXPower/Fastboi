#include "Utility.h"
#include <cstdio>
#include "SDL/SDL.h"


void print_rect(const SDL_Rect& rect) {
   printf("(x,y) (%i,%i) : (w, h) (%i,%i)", rect.x, rect.y, rect.w, rect.h);
}

void print_rect(const SDL_FRect& rect) {
   printf("(x,y) (%f,%f) : (w, h) (%f,%f)", rect.x, rect.y, rect.w, rect.h);
}