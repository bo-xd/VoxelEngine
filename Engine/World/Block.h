#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include "../utils/MathUtil.h"

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
} Chunk;


Block* CreateBlock(vec3 pos, block_type type);
Chunk* CreateChunk(vec3 pos, int size, float voxelsize);
vec3 BlockTypeToColor(block_type type);

#endif
