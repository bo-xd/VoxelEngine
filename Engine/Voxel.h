#ifndef VOXEL_H
#define VOXEL_H

#include <GL/glew.h>
#include "Shaderer.h"
#include "utils/MathUtil.h"

typedef struct {
    GLuint VAO;
    GLuint VBO;
} VoxelMesh;

VoxelMesh CreateVoxelMesh(float size);
void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection);
void Shader_SetMat4(shader* s, const char* name, const mat4* mat);

#endif
