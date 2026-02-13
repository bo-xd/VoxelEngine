#include "Window.h"
#include "Camera.h"
#include <GL/glew.h>
#include "Player/Player.h"
#include "Renderer.h"
#include "Shaderer.h"
#include "World/Block.h"
#include "World/Lighting.h"
#include "ui/text.h"
#include "utils/FreeUtil.h"
#include "utils/MathUtil.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 32
#define VIEW_DISTANCE 100.0f
#define RENDER_DISTANCE 5
#define MAX_CHUNKS 64

int CreateWindow(const char *title, int WIDTH, int HEIGHT) {
  window_t Window = {0};
  Window.WIDTH = WIDTH;
  Window.HEIGHT = HEIGHT;

  if (SDL_Init(SDL_INIT_VIDEO) == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL init failed: %s\n",
                 SDL_GetError());
    return 1;
  }

  if (TTF_Init() == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF init failed: %s\n",
                 SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  Window.window = SDL_CreateWindow(title, WIDTH, HEIGHT,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!Window.window) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window creation failed: %s\n",
                 SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  Window.context = SDL_GL_CreateContext(Window.window);
  if (!Window.context) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "OpenGL context failed: %s\n",
                 SDL_GetError());
    SDL_DestroyWindow(Window.window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  glewExperimental = GL_TRUE;
  if (glewInit() == GLEW_OK) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GLEW init failed\n");
    SDL_DestroyWindow(Window.window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  SDL_SetWindowRelativeMouseMode(Window.window, true);

  InitWorldSeed(rand() % 6);

  Player player;
  InitPlayer(&player,
             (vec3){CHUNK_SIZE * 0.2f * 0.5f, 25.0f, CHUNK_SIZE * 0.2f * 0.5f});

  DirectionalLight sunlight = {.direction = {-0.2f, -1.0f, -0.4f},
                               .color = {1.0f, 0.98f, 0.95f},
                               .ambient = 0.4f,
                               .diffuse = 0.6f,
                               .specular = 0.2f};

  VoxelMesh cubeMesh = CreateVoxelMesh(0.2f);
  shader cubeShader =
      Shader_Load("Shaders/voxel/cube.vert", "Shaders/voxel/cube.frag");

  ChunkSlot *chunkSlots = (ChunkSlot *)calloc(MAX_CHUNKS, sizeof(ChunkSlot));
  Chunk **chunkPointers = (Chunk **)malloc(MAX_CHUNKS * sizeof(Chunk *));

  UpdateChunkLoading(chunkSlots, MAX_CHUNKS, player.position, 0.2f, CHUNK_SIZE,
                     RENDER_DISTANCE);

  shader skyShader =
      Shader_Load("Shaders/skybox/sky.vert", "Shaders/skybox/sky.frag");
  SkyDome skyDome = CreateSkyDome(64, 32, (vec3){0.5f, 0.7f, 0.95f},
                                  (vec3){0.9f, 0.95f, 1.0f});

  shader fontShader =
      Shader_Load("Shaders/Text/text.vert", "Shaders/Text/text.frag");
  TTF_Font *font = TTF_OpenFont("textures/fonts/VCR.ttf", 24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s\n",
                 SDL_GetError());
  }
  SDL_Color yellow = {255, 255, 0, 255};
  TextTexture fpsTex = {0};
  TextTexture posTex = {0};

  char fpsText[32];
  char posText[64];
  int frames = 0;
  float fpsTimer = 0.0f;
  float chunkUpdateTimer = 0.0f;

  snprintf(fpsText, sizeof(fpsText), "FPS: 0");
  fpsTex = CreateTextTexture(font, fpsText, yellow);

  snprintf(posText, sizeof(posText), "Pos: (0.0, 0.0, 0.0)");
  posTex = CreateTextTexture(font, posText, yellow);

  Window.Running = true;
  int lastTicks = SDL_GetTicks();

  while (Window.Running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        Window.Running = false;
      if (event.type == SDL_EVENT_MOUSE_MOTION)
        ProcessPlayerMouseMovement(&player, event.motion.xrel,
                                   event.motion.yrel);
      if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        WIDTH = event.window.data1;
        HEIGHT = event.window.data2;
        glViewport(0, 0, WIDTH, HEIGHT);
      }
    }

    int nowTicks = SDL_GetTicks();
    float deltaTime = (nowTicks - lastTicks) / 1000.0f;
    lastTicks = nowTicks;

    ProcessPlayerInput(&player, deltaTime);

    int activeChunks = 0;
    for (int i = 0; i < MAX_CHUNKS; i++) {
      if (chunkSlots[i].loaded) {
        chunkPointers[activeChunks++] = chunkSlots[i].chunk;
      }
    }

    UpdatePlayer(&player, deltaTime, chunkPointers, activeChunks, CHUNK_SIZE,
                 0.2f);

    chunkUpdateTimer += deltaTime;
    if (chunkUpdateTimer >= 0.5f) {
      UpdateChunkLoading(chunkSlots, MAX_CHUNKS, player.position, 0.2f,
                         CHUNK_SIZE, RENDER_DISTANCE);
      chunkUpdateTimer = 0.0f;
    }

    frames++;
    fpsTimer += deltaTime;
    if (fpsTimer >= 0.5f) {
      snprintf(fpsText, sizeof(fpsText), "FPS: %d", (int)(frames / fpsTimer));
      frames = 0;
      fpsTimer = 0.0f;

      FreeTextTexture(&fpsTex);
      fpsTex = CreateTextTexture(font, fpsText, yellow);

      snprintf(posText, sizeof(posText), "Pos: (%.1f, %.1f, %.1f)",
               player.position.x, player.position.y, player.position.z);
      FreeTextTexture(&posTex);
      posTex = CreateTextTexture(font, posText, yellow);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (float)WIDTH / HEIGHT;
    mat4 projection = Perspective(60.0f, aspect, 0.1f, 200.0f);
    vec3 front = CameraFront(&player.cam);
    vec3 target = Vec3Add(player.cam.pos, front);
    vec3 up = {0.0f, 1.0f, 0.0f};
    mat4 view = LookAt(player.cam.pos, target, up);

    Shader_Use(&skyShader);
    glUniform3f(glGetUniformLocation(skyShader.id, "topColor"), 0.53f, 0.81f,
                0.98f);
    glUniform3f(glGetUniformLocation(skyShader.id, "horizonColor"), 1.0f, 1.0f,
                1.0f);
    DrawSkyDome(&skyDome, &skyShader, view, projection);

    Shader_Use(&cubeShader);
    SetDirectionalLightUniforms(&sunlight, cubeShader.id, player.cam.pos);

    for (int i = 0; i < MAX_CHUNKS; i++) {
      if (chunkSlots[i].loaded && chunkSlots[i].chunk) {
        DrawChunk(chunkSlots[i].chunk, &cubeMesh, &cubeShader, view, projection,
                  CHUNK_SIZE, player.cam.pos, VIEW_DISTANCE);
      }
    }

    glDisable(GL_DEPTH_TEST);
    if (fpsTex.texture != 0) {
      RenderTextTexture(&fontShader, &fpsTex, 10.0f, 10.0f, WIDTH, HEIGHT);
    }
    if (posTex.texture != 0) {
      RenderTextTexture(&fontShader, &posTex, 10.0f, 40.0f, WIDTH, HEIGHT);
    }
    glEnable(GL_DEPTH_TEST);

    SDL_GL_SwapWindow(Window.window);
  }

  FreeShader(1, &cubeMesh.VBO, &cubeShader);
  FreeSkyDome(&skyDome);
  FreeTextTexture(&fpsTex);
  FreeTextTexture(&posTex);
  if (font)
    TTF_CloseFont(font);
  TTF_Quit();

  FreeAllChunks(chunkSlots, MAX_CHUNKS, CHUNK_SIZE);
  free(chunkSlots);
  free(chunkPointers);

  SDL_GL_DestroyContext(Window.context);
  SDL_DestroyWindow(Window.window);
  SDL_Quit();

  return 0;
}
