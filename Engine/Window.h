#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct {
    bool Running;
    SDL_Window* window;
    SDL_GLContext context;
    int WIDTH;
    int HEIGHT;
    int fps;
} window_t;

int CreateWindow(const char *title, int WIDTH, int HEIGHT);

#endif
