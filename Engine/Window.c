#include "Window.h"
#include "Camera.h"
#include "Voxel.h"
#include "utils/MathUtil.h"
#include <SDL3/SDL.h>
#include <GL/gl.h>
#include <math.h>

int CreateWindow(const char *title, int WIDTH, int HEIGHT) {
    window_t Window = {
        .Running = false,
        .window = NULL,
        .context = NULL,
        .WIDTH = WIDTH,
        .HEIGHT = HEIGHT,
        .fps = 60
    };

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    Window.window = SDL_CreateWindow(
        title,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!Window.window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Window.context = SDL_GL_CreateContext(Window.window);
    if (!Window.context) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "OpenGL context failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(Window.window);
        SDL_Quit();
        return 1;
    }

    // Projection
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)WIDTH / HEIGHT;
    float fov = 60.0f;
    float near = 0.1f, far = 100.0f;

    float top = tanf(fov * M_PI / 360.0f) * near;
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    glFrustum(left, right, bottom, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);

    camera cam;
    InitCamera(&cam, (vec3){1.0f, 1.0f, 5.0f});

    Window.Running = true;
    int lastTicks = SDL_GetTicks();

    while (Window.Running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                Window.Running = false;
        }

        int nowTicks = SDL_GetTicks();
        float deltaTime = (nowTicks - lastTicks) / 1000.0f;
        lastTicks = nowTicks;

        ProcessInput(&cam, deltaTime);
        ApplyCameraView(&cam);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CreateVoxel(1, 1, 1, 1);

        SDL_GL_SwapWindow(Window.window);
    }

    SDL_GL_DestroyContext(Window.context);
    SDL_DestroyWindow(Window.window);
    SDL_Quit();

    return 0;
}
