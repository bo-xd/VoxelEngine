#include "Shaderer.h"
#include <GL/gl.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL3_image/SDL_image.h>

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


GLuint LoadCubeMap(const char* faces[], int count) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (int i = 0; i < count; i++) {
        SDL_Surface* surface = SDL_LoadBMP(faces[i]);
        if (!surface) {
            printf("Failed to load cubemap texture %s: %s\n", faces[i], SDL_GetError());
            continue;
        }

        int bpp = SDL_BITSPERPIXEL(SDL_GetSurfaceProperties(surface)) / 8;
        GLenum format = (bpp == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            format,
            surface->w,
            surface->h,
            0,
            format,
            GL_UNSIGNED_BYTE,
            surface->pixels
        );

        SDL_DestroySurface(surface);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
