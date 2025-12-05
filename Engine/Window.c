#include "Window.h"
#include "Camera.h"
#include "Renderer.h"
#include "World/Block.h"
#include "utils/MathUtil.h"
#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <stdlib.h>
#include "Shaderer.h"
#include "utils/FreeUtil.h"
#include "World/Lighting.h"

#define CHUNK_SIZE 32
#define VIEW_DISTANCE 32.0f

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
    InitCamera(&cam, (vec3){0.0f, 2.0f, 0.0f});

    DirectionalLight sunlight = {
        .direction = { -0.3f, -1.0f, -0.3f },
        .color = { 1.0f, 1.0f, 1.0f },
        .ambient = 0.2f,
        .diffuse = 0.7f,
        .specular = 0.5f
    };

    VoxelMesh cubeMesh = CreateVoxelMesh(0.2f);
    shader cubeShader = Shader_Load("Shaders/voxel/cube.vert", "Shaders/voxel/cube.frag");

    Chunk* chunks[1];
    chunks[0] = CreateChunk((vec3){0,0,0}, CHUNK_SIZE, 0.2f);
    BuildChunkMesh(chunks[0], CHUNK_SIZE, 0.2f);

    shader skyShader = Shader_Load("Shaders/skybox/sky.vert", "Shaders/skybox/sky.frag");

    SkyDome skyDome = CreateSkyDome(64, 32, (vec3){0.53f, 0.81f, 0.98f}, (vec3){1.0f, 1.0f, 1.0f});

    Window.Running = true;
    int lastTicks = SDL_GetTicks();

    while (Window.Running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                Window.Running = false;

            if (event.type == SDL_EVENT_MOUSE_MOTION)
                ProcessMouseMovement(&cam, event.motion.xrel, event.motion.yrel);

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                WIDTH = event.window.data1;
                HEIGHT = event.window.data2;
                glViewport(0,0,WIDTH,HEIGHT);
            }
        }

        int nowTicks = SDL_GetTicks();
        float deltaTime = (nowTicks - lastTicks) / 1000.0f;
        lastTicks = nowTicks;

        ProcessInput(&cam, deltaTime);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)WIDTH / HEIGHT;
        mat4 projection = Perspective(60.0f, aspect, 0.1f, 100.0f);

        vec3 front = CameraFront(&cam);
        vec3 target = Vec3Add(cam.pos, front);
        vec3 up = {0.0f, 1.0f, 0.0f};
        mat4 view = LookAt(cam.pos, target, up);

        Shader_Use(&skyShader);
        GLint topLoc = glGetUniformLocation(skyShader.id, "topColor");
        GLint horLoc = glGetUniformLocation(skyShader.id, "horizonColor");
        glUniform3f(topLoc, 0.53f, 0.81f, 0.98f);
        glUniform3f(horLoc, 1.0f, 1.0f, 1.0f);

        DrawSkyDome(&skyDome, &skyShader, view, projection);

        Shader_Use(&cubeShader);
        SetDirectionalLightUniforms(&sunlight, cubeShader.id, cam.pos);

        for (int i = 0; i < 1; i++) {
            DrawChunk(chunks[i], &cubeMesh, &cubeShader, view, projection, CHUNK_SIZE, cam.pos, VIEW_DISTANCE);
        }

        SDL_GL_SwapWindow(Window.window);
    }

    FreeShader(1, &cubeMesh.VBO, &cubeShader);
    FreeSkyDome(&skyDome);

    for (int i = 0; i < 1; i++) {
        FreeChunkMesh(chunks[i]);
        for (int x=0;x<CHUNK_SIZE;x++)
         for (int y=0;y<CHUNK_SIZE;y++)
          for (int z=0;z<CHUNK_SIZE;z++)
            if (chunks[i]->blocks[x][y][z]) free(chunks[i]->blocks[x][y][z]);
        free(chunks[i]);
    }

    SDL_GL_DestroyContext(Window.context);
    SDL_DestroyWindow(Window.window);
    SDL_Quit();

    return 0;
}
