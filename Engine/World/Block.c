#include "Block.h"
#include <stdlib.h>
#include <math.h>

Block* CreateBlock(vec3 pos, block_type type) {
    Block* b = (Block*)malloc(sizeof(Block));
    if (!b) return NULL;
    b->active = true;
    b->type = type;
    b->position = pos;
    b->color = BlockTypeToColor(type);
    return b;
}

Chunk* CreateChunk(vec3 pos, int size, float voxelSize) {
    Chunk* c = (Chunk*)malloc(sizeof(Chunk));
    if (!c) return NULL;
    c->position = pos;
    c->meshVAO = 0;
    c->meshVBO = 0;
    c->meshVertexCount = 0;
    for (int x = 0; x < size; x++)
        for (int y = 0; y < size; y++)
            for (int z = 0; z < size; z++)
                c->blocks[x][y][z] = (y == 0) ?
                    CreateBlock((vec3){pos.x + x*voxelSize, pos.y + y*voxelSize, pos.z + z*voxelSize}, BLOCK_GRASS)
                    : NULL;
    return c;
}

vec3 BlockTypeToColor(block_type type) {
    vec3 baseColor;
    switch(type) {
        case BLOCK_GRASS: baseColor = (vec3){0.4f, 0.8f, 0.4f}; break;
        case BLOCK_STONE: baseColor = (vec3){0.6f, 0.6f, 0.65f}; break;
        case BLOCK_WOOD:  baseColor = (vec3){0.55f, 0.35f, 0.2f}; break;
        default:          baseColor = (vec3){1.0f, 1.0f, 1.0f}; break;
    }
    float variation = 0.05f;
    baseColor.x += ((float)rand() / RAND_MAX - 0.5f) * 2.0f * variation;
    baseColor.y += ((float)rand() / RAND_MAX - 0.5f) * 2.0f * variation;
    baseColor.z += ((float)rand() / RAND_MAX - 0.5f) * 2.0f * variation;
    baseColor.x = fminf(fmaxf(baseColor.x, 0.0f), 1.0f);
    baseColor.y = fminf(fmaxf(baseColor.y, 0.0f), 1.0f);
    baseColor.z = fminf(fmaxf(baseColor.z, 0.0f), 1.0f);
    return baseColor;
}
