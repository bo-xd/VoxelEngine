#ifndef VOXEL_H
#define VOXEL_H

#include <GL/glew.h>
#include "Shaderer.h"
#include "utils/MathUtil.h"

typedef struct {
    GLuint VAO;
    GLuint VBO;
} VoxelMesh;

typedef struct {
    int voxels[32][32][32];
    vec3 position;
} chunk;

typedef struct {
    GLuint VAO, VBO;
} Skybox;


VoxelMesh CreateVoxelMesh(float size);
void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection);
chunk* CreateChunk(vec3 pos, int size);
void DrawChunk(const chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection, int size);
void Shader_SetMat4(shader* s, const char* name, const mat4* mat);
Skybox CreateSkybox();
void DrawSkybox(Skybox* sb, shader* s, GLuint texture, mat4 view, mat4 projection);




#endif
