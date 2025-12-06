#include "Shaderer.h"
#include <GL/gl.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL.h>
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


void Shader_SetMat4(shader* s, const char* name, const mat4* mat) {
    GLint loc = glGetUniformLocation(s->id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat->m);
}

GLuint LoadTexture(const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Failed to load texture %s: %s\n", filename, SDL_GetError());
        return 0;
    }

    GLenum format;
    Uint32 pixfmt = surface->format;

    int bpp = SDL_BITSPERPIXEL(pixfmt);
    if (bpp == 24) {
        format = GL_RGB;
    } else if (bpp == 32) {
        format = GL_RGBA;
    } else {
        printf("Unsupported image format: %s\n", filename);
        SDL_DestroySurface(surface);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    SDL_DestroySurface(surface);
    return textureID;
}
