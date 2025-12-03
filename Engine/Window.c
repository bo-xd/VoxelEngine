#include "Window.h"
#include "Camera.h"
#include "Voxel.h"
#include "utils/MathUtil.h"
#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <math.h>

int CreateWindow(const char *title, int WIDTH, int HEIGHT) {
    window_t Window = {0};
    Window.WIDTH = WIDTH;
    Window.HEIGHT = HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    Window.window = SDL_CreateWindow(title, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GLEW init failed\n");
        SDL_DestroyWindow(Window.window);
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowRelativeMouseMode(Window.window, true);
    glEnable(GL_DEPTH_TEST);

    camera cam;
    InitCamera(&cam, (vec3){1.0f, 1.0f, 5.0f});

    int chunksize = 10;
    VoxelMesh cubeMesh = CreateVoxelMesh(1.0f);
    shader cubeShader = Shader_Load("Shaders/voxel/cube.vert", "Shaders/voxel/cube.frag");
    chunk* Chonk = CreateChunk((vec3){0,0,0}, chunksize);

    Window.Running = true;
    int lastTicks = SDL_GetTicks();

    while (Window.Running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                Window.Running = false;
            if (event.type == SDL_EVENT_MOUSE_MOTION)
                ProcessMouseMovement(&cam, event.motion.xrel, event.motion.yrel);
        }

        int nowTicks = SDL_GetTicks();
        float deltaTime = (nowTicks - lastTicks) / 1000.0f;
        lastTicks = nowTicks;

        ProcessInput(&cam, deltaTime);

        glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)WIDTH / HEIGHT;
        mat4 projection = Perspective(60.0f, aspect, 0.1f, 100.0f);

        vec3 front = CameraFront(&cam);
        vec3 target = { cam.pos.x + front.x, cam.pos.y + front.y, cam.pos.z + front.z };
        vec3 up = {0.0f, 1.0f, 0.0f};
        mat4 view = LookAt(cam.pos, target, up);

        // DrawVoxel(&cubeMesh, &cubeShader, (vec3){0, 0, 0}, view, projection);
        DrawChunk(Chonk, &cubeMesh, &cubeShader, view, projection, chunksize);

        SDL_GL_SwapWindow(Window.window);
    }

    glDeleteBuffers(1, &cubeMesh.VBO);
    glDeleteVertexArrays(1, &cubeMesh.VAO);
    Shader_Destroy(&cubeShader);

    SDL_GL_DestroyContext(Window.context);
    SDL_DestroyWindow(Window.window);
    SDL_Quit();

    return 0;
}
