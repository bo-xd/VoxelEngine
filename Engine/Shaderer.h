#ifndef SHADERER_H
#define SHADERER_H

#include <GL/glew.h>

typedef struct {
    GLuint id;
} shader;

shader Shader_Load(const char* vertPath, const char* fragPath);
void Shader_Destroy(shader* s);
void Shader_Use(shader* s);

GLuint LoadCubeMap(const char* faces[], int count);

void Shader_SetInt(shader* s, const char* name, int value);
void Shader_SetFloat(shader* s, const char* name, float value);


#endif
