#include "Shaderer.h"
#include <GL/gl.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL3_image/SDL_image.h>
#include <GL/glu.h>

static char* ReadFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Shaderer: Failed to open %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = 0;

    fclose(f);
    return buffer;
}

static GLuint Compile(GLenum type, const char* src) {

    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        printf("Shaderer: Shader compile error:\n%s\n", log);
    }

    return shader;
}

shader Shader_Load(const char* vertPath, const char* fragPath) {
    shader s = {0};
    char* vertSrc = ReadFile(vertPath);

    char* fragSrc = ReadFile(fragPath);

    if (!vertSrc || !fragSrc) {
        printf("Shaderer: Source read failed.\n");
        return s;
    }

    GLuint vert = Compile(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = Compile(GL_FRAGMENT_SHADER, fragSrc);

    s.id = glCreateProgram();
    glAttachShader(s.id, vert);
    glAttachShader(s.id, frag);
    glLinkProgram(s.id);

    GLint success;
    glGetProgramiv(s.id, GL_LINK_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetProgramInfoLog(s.id, sizeof(log), NULL, log);
        printf("Shaderer: Program link error:\n%s\n", log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    free(vertSrc);
    free(fragSrc);

    return s;
}

void Shader_Destroy(shader* s) {
    if (s->id) {
        glDeleteProgram(s->id);
        s->id = 0;
    }
}

void Shader_Use(shader* s) {
    glUseProgram(s->id);
}

void Shader_SetInt(shader* s, const char* name, int value) {
    GLint loc = glGetUniformLocation(s->id, name);
    glUniform1i(loc, value);
}

void Shader_SetFloat(shader* s, const char* name, float value) {
    GLint loc = glGetUniformLocation(s->id, name);
    glUniform1f(loc, value);
}

GLuint LoadTexture(const char* filename, int width, int height) {
  GLuint texture;
  unsigned char* data;
  FILE *file;

  file = fopen(filename, "rb");
  if (file == NULL) {
    perror("couldnt open file");
    return 0;
  }

  data = (unsigned char*)malloc(width * height * 3);
  fread(data, width * height * 3, 1, file);

  fclose(file);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


  glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
  free(data);

  return texture;
}

unsigned char* ExtractFace(SDL_Surface* src, int x, int y, int size) {
    unsigned char* buffer = malloc(size * size * 4);

    for (int row = 0; row < size; row++) {
        memcpy(
            buffer + row * size * 4,
            (unsigned char*)src->pixels
                + (y + row) * src->pitch
                + (x * 4),
            size * 4
        );
    }

    return buffer;
}

GLuint LoadCubemapAtlas(const char* filename) {
    SDL_Surface* atlas = IMG_Load(filename);
    if (!atlas) {
        printf("Failed to load atlas %s: %s\n", filename, SDL_GetError());
        return 0;
    }

    int w = atlas->w;
    int h = atlas->h;
    int face = h / 3;

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    int ox[6] = {0, 1, 2, 3, 1, 1};
    int oy[6] = {1, 1, 1, 1, 0, 2};

    for (int i = 0; i < 6; i++) {
        unsigned char* data = ExtractFace(atlas, ox[i] * face, oy[i] * face, face);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGBA, face, face, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    SDL_DestroySurface(atlas);
    return texID;
}
