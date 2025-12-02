#include "Shaderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

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
