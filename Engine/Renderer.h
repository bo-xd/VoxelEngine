#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "Shaderer.h"
#include "utils/MathUtil.h"
#include "World/Block.h"

typedef struct {
    GLuint VAO;
    GLuint VBO;
    GLuint vertexCount;
} VoxelMesh;

typedef struct {
    GLuint VAO;
    GLuint VBO;
} Skybox;

VoxelMesh CreateVoxelMesh(float size);
void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection, vec3 color);

void DrawChunk(const Chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection, int size, vec3 camPos, float maxDist);
void Shader_SetMat4(shader* s, const char* name, const mat4* mat);

Skybox CreateSkybox();
void DrawSkybox(Skybox* sb, shader* s, GLuint texture, mat4 view, mat4 projection);

#endif
