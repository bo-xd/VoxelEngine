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
