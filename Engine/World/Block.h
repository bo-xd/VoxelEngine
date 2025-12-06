#ifndef BLOCK_H
#define BLOCK_H
#include <stdbool.h>
#include "../utils/MathUtil.h"
#include <GL/gl.h>

typedef enum {
    BLOCK_GRASS,
    BLOCK_STONE,
    BLOCK_WOOD
} block_type;

typedef struct {
    bool active;
    block_type type;
    vec3 position;
    vec3 color;
} Block;

typedef struct {
    Block* blocks[32][32][32];
    vec3 position;
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshVertexCount;
} Chunk;

typedef struct {
    Chunk* chunk;
    int chunkX;
    int chunkZ;
    bool loaded;
} ChunkSlot;

Block* CreateBlock(vec3 pos, block_type type);
Chunk* CreateChunk(vec3 pos, int size, float voxelSize);
vec3 BlockTypeToColor(block_type type);

void InitWorldSeed(int seed);
int GetWorldSeed(void);

Chunk* GetOrCreateChunk(ChunkSlot* slots, int maxSlots, int chunkX, int chunkZ, float voxelSize, int chunkSize);
void UpdateChunkLoading(ChunkSlot* slots, int maxSlots, vec3 playerPos, float voxelSize, int chunkSize, int renderDist);
void FreeAllChunks(ChunkSlot* slots, int maxSlots, int chunkSize);

#endif
