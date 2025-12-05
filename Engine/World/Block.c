#include "Block.h"
#include <stdlib.h>

Block* CreateBlock(vec3 pos, block_type type) {
    Block* b = (Block*)malloc(sizeof(Block));
    if (!b) return NULL;

    b->active = true;
    b->type = type;
    b->position = pos;

    return b;
}

Chunk* CreateChunk(vec3 pos, int size) {
    Chunk* c = (Chunk*)malloc(sizeof(Chunk));
    if (!c) return NULL;

    c->position = pos;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            for (int z = 0; z < size; z++) {
                if (y == 0) {
                    c->blocks[x][y][z] = CreateBlock(
                        (vec3){pos.x + x, pos.y + y, pos.z + z}, BLOCK_GRASS
                    );
                } else {
                    c->blocks[x][y][z] = NULL;
                }
            }
        }
    }

    return c;
}

vec3 BlockTypeToColor(block_type type) {
    switch(type) {
        case BLOCK_GRASS: return (vec3){0.0f, 1.0f, 0.0f};
        case BLOCK_STONE: return (vec3){0.5f, 0.5f, 0.5f};
        case BLOCK_WOOD:  return (vec3){0.55f, 0.27f, 0.07f};
        default:          return (vec3){1.0f, 1.0f, 1.0f};
    }
}
