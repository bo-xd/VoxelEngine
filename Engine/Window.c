#include "Window.h"
#include <GL/gl.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

int CreateWindow(const char *title, int WIDTH, int HEIGHT) {
    window_t Window = {
        .Running = false,
        .window = NULL,
        .context = NULL,
        .WIDTH = WIDTH,
        .HEIGHT = HEIGHT,
        .fps = 60
    };

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize: %s\n", SDL_GetError());
        return 1;
    }

    Window.window = SDL_CreateWindow(title, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

    if (Window.window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window '%s'. SDL_Error: %s\n", title, SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Window.context = SDL_GL_CreateContext(Window.window);
    if (Window.context == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "OpenGL context could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(Window.window);
        SDL_Quit();
        return 1;
    }

    Window.Running = true;

    while (Window.Running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                Window.Running = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(Window.window);
    }

    if (Window.window != NULL) {
        SDL_DestroyWindow(Window.window);
    }
    SDL_Quit();

    return 0;
}
