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
    GLuint vertexCount;
    vec3 topColor;
    vec3 bottomColor;
} SkyDome;

VoxelMesh CreateVoxelMesh(float size);
void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection, vec3 color);
bool IsFaceVisible(Chunk* c, int x, int y, int z, int dx, int dy, int dz);
void DrawChunk(const Chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection, int size, vec3 camPos, float maxDist);
void BuildChunkMesh(Chunk* c, int size, float voxelSize);
void FreeChunkMesh(Chunk* c);

SkyDome CreateSkyDome(int slices, int stacks, vec3 topColor, vec3 bottomColor);
void DrawSkyDome(SkyDome* dome, shader* s, mat4 view, mat4 projection);
void FreeSkyDome(SkyDome* dome);

#endif
