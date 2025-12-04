#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL3/SDL_surface.h>

typedef struct shader {
    GLuint id;
} shader;

shader Shader_Load(const char* vertPath, const char* fragPath);
void Shader_Destroy(shader* s);
void Shader_Use(shader* s);
void Shader_SetInt(shader* s, const char* name, int value);
void Shader_SetFloat(shader* s, const char* name, float value);

unsigned char* ExtractFace(SDL_Surface* src, int x, int y, int size);
GLuint LoadCubemapAtlas(const char* filename);
