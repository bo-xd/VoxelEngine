#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

typedef struct window {
    bool Running;

    SDL_Window* window;
    SDL_GLContext context;

    int fps;
    int WIDTH;
    int HEIGHT;
} window_t;

int CreateWindow(const char* title, int WIDTH, int HEIGHT);

#endif
