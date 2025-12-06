#include "Block.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

static int hash(int x, int y, int seed) {
    int h = seed;
    h = (h ^ x) * 0x27d4eb2d;
    h = (h ^ y) * 0x27d4eb2d;
    h = (h ^ (h >> 15)) * 0x27d4eb2d;
    return h;
}

static float smoothstep(float t) {
    return t * t * (3.0f - 2.0f * t);
}

static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static float gradient(int hash, float x, float y) {
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float perlinNoise2D(float x, float y, int seed) {
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    float n00 = gradient(hash(x0, y0, seed), sx, sy);
    float n10 = gradient(hash(x1, y0, seed), sx - 1.0f, sy);
    float n01 = gradient(hash(x0, y1, seed), sx, sy - 1.0f);
    float n11 = gradient(hash(x1, y1, seed), sx - 1.0f, sy - 1.0f);

    float u = smoothstep(sx);
    float v = smoothstep(sy);

    float nx0 = lerp(n00, n10, u);
    float nx1 = lerp(n01, n11, u);

    return lerp(nx0, nx1, v);
}

float perlinNoise(float x, float z, int seed, int octaves, float persistence) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++) {
        total += perlinNoise2D(x * frequency, z * frequency, seed + i) * amplitude;

        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

float noise2D(int x, int z, int seed) {
    int n = x + z * 57 + seed * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float caveNoise(float x, float y, float z, int seed) {
    int ix = (int)x;
    int iy = (int)y;
    int iz = (int)z;

    int n = ix + iy * 57 + iz * 131 + seed * 263;
    n = (n << 13) ^ n;
    float noise = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);

    return noise;
}

block_type getBlockType(int worldY, int surfaceHeight, float temperature) {
    if (worldY > surfaceHeight) return BLOCK_GRASS;

    int depthBelowSurface = surfaceHeight - worldY;

    if (depthBelowSurface == 0) {
        if (surfaceHeight > 50) {
            return BLOCK_STONE;
        } else if (temperature < -0.3f) {
            return BLOCK_STONE;
        } else {
            return BLOCK_GRASS;
        }
    } else if (depthBelowSurface < 4) {
        return BLOCK_GRASS;
    } else {
        return BLOCK_STONE;
    }
}

static int g_worldSeed = 0;

void InitWorldSeed(int seed) {
    g_worldSeed = seed;
}

int GetWorldSeed() {
    if (g_worldSeed == 0) {
        g_worldSeed = time(NULL) % 10000;
    }
    return g_worldSeed;
}

Chunk* CreateChunk(vec3 pos, int size, float voxelSize) {
    int worldSeed = GetWorldSeed();

    Chunk* c = (Chunk*)malloc(sizeof(Chunk));
    if (!c) return NULL;

    c->position = pos;
    c->meshVAO = 0;
    c->meshVBO = 0;
    c->meshVertexCount = 0;

    for (int x = 0; x < size; x++)
        for (int y = 0; y < size; y++)
            for (int z = 0; z < size; z++)
                c->blocks[x][y][z] = NULL;

    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            float worldX = (pos.x / voxelSize) + x;
            float worldZ = (pos.z / voxelSize) + z;

            float continentalShape = perlinNoise(worldX * 0.0008f, worldZ * 0.0008f, worldSeed, 4, 0.5f);
            float mountains = perlinNoise(worldX * 0.003f, worldZ * 0.003f, worldSeed + 1, 6, 0.55f);
            float hills = perlinNoise(worldX * 0.01f, worldZ * 0.01f, worldSeed + 2, 4, 0.5f);
            float details = perlinNoise(worldX * 0.04f, worldZ * 0.04f, worldSeed + 3, 3, 0.4f);

            float baseHeight = continentalShape * 0.3f + mountains * 0.4f + hills * 0.2f + details * 0.1f;

            float heightMultiplier = 1.0f;
            if (baseHeight > 0.2f) {
                heightMultiplier = 1.0f + powf((baseHeight - 0.2f) / 0.8f, 2.5f) * 3.0f;
            }

            int surfaceHeight = (int)((baseHeight + 1.0f) * 0.5f * 35.0f * heightMultiplier + 8.0f);

            surfaceHeight = fmaxf(3, fminf(size - 1, surfaceHeight));

            float temperature = perlinNoise(worldX * 0.003f, worldZ * 0.003f, worldSeed + 100, 2, 0.5f);

            for (int y = 0; y <= surfaceHeight; y++) {
                float worldY = (pos.y / voxelSize) + y;

                float cave1 = caveNoise(worldX * 0.05f, worldY * 0.05f, worldZ * 0.05f, worldSeed + 50);
                float cave2 = caveNoise(worldX * 0.04f, worldY * 0.04f, worldZ * 0.04f, worldSeed + 75);

                bool isCave = (cave1 > 0.65f && cave2 > 0.6f && y < surfaceHeight - 3);

                if (!isCave) {
                    block_type type = getBlockType(y, surfaceHeight, temperature);
                    vec3 blockPos = {
                        pos.x + x * voxelSize,
                        pos.y + y * voxelSize,
                        pos.z + z * voxelSize
                    };
                    c->blocks[x][y][z] = CreateBlock(blockPos, type);
                }
            }

            if (c->blocks[x][surfaceHeight][z] &&
                c->blocks[x][surfaceHeight][z]->type == BLOCK_GRASS &&
                surfaceHeight < size - 10 && surfaceHeight > 5) {

                float treeNoise = noise2D((int)worldX, (int)worldZ, worldSeed + 200);
                if (treeNoise > 0.90f && temperature > -0.2f) {
                    int treeHeight = 4 + (rand() % 3);

                    for (int ty = 1; ty <= treeHeight && (surfaceHeight + ty) < size; ty++) {
                        if (!c->blocks[x][surfaceHeight + ty][z]) {
                            vec3 blockPos = {
                                pos.x + x * voxelSize,
                                pos.y + (surfaceHeight + ty) * voxelSize,
                                pos.z + z * voxelSize
                            };
                            c->blocks[x][surfaceHeight + ty][z] = CreateBlock(blockPos, BLOCK_WOOD);
                        }
                    }

                    int leafStartY = surfaceHeight + treeHeight - 1;
                    for (int ly = 0; ly <= 2; ly++) {
                        int currentY = leafStartY + ly;
                        if (currentY >= size) break;

                        int radius = (ly == 1) ? 2 : 1;
                        for (int lx = -radius; lx <= radius; lx++) {
                            for (int lz = -radius; lz <= radius; lz++) {
                                if (radius == 2 && abs(lx) == 2 && abs(lz) == 2) continue;

                                int leafX = x + lx;
                                int leafZ = z + lz;

                                if (leafX >= 0 && leafX < size && leafZ >= 0 && leafZ < size) {
                                    if (!c->blocks[leafX][currentY][leafZ]) {
                                        vec3 blockPos = {
                                            pos.x + leafX * voxelSize,
                                            pos.y + currentY * voxelSize,
                                            pos.z + leafZ * voxelSize
                                        };
                                        c->blocks[leafX][currentY][leafZ] = CreateBlock(blockPos, BLOCK_GRASS);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return c;
}

Block* CreateBlock(vec3 pos, block_type type) {
    Block* b = (Block*)malloc(sizeof(Block));
    if (!b) return NULL;
    b->active = true;
    b->type = type;
    b->position = pos;
    b->color = BlockTypeToColor(type);
    return b;
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

Chunk* GetOrCreateChunk(ChunkSlot* slots, int maxSlots, int chunkX, int chunkZ, float voxelSize, int chunkSize) {
    for (int i = 0; i < maxSlots; i++) {
        if (slots[i].loaded && slots[i].chunkX == chunkX && slots[i].chunkZ == chunkZ) {
            return slots[i].chunk;
        }
    }

    int emptySlot = -1;
    for (int i = 0; i < maxSlots; i++) {
        if (!slots[i].loaded) {
            emptySlot = i;
            break;
        }
    }

    if (emptySlot == -1) {
        emptySlot = 0;
        if (slots[emptySlot].chunk) {
            extern void FreeChunkMesh(Chunk* chunk);
            FreeChunkMesh(slots[emptySlot].chunk);
            for (int x = 0; x < chunkSize; x++)
                for (int y = 0; y < chunkSize; y++)
                    for (int z = 0; z < chunkSize; z++)
                        if (slots[emptySlot].chunk->blocks[x][y][z])
                            free(slots[emptySlot].chunk->blocks[x][y][z]);
            free(slots[emptySlot].chunk);
        }
    }

    float chunkWorldSize = chunkSize * voxelSize;
    vec3 chunkPos = {
        chunkX * chunkWorldSize,
        0.0f,
        chunkZ * chunkWorldSize
    };

    slots[emptySlot].chunk = CreateChunk(chunkPos, chunkSize, voxelSize);
    slots[emptySlot].chunkX = chunkX;
    slots[emptySlot].chunkZ = chunkZ;
    slots[emptySlot].loaded = true;

    extern void BuildChunkMesh(Chunk* chunk, int size, float voxelSize);
    BuildChunkMesh(slots[emptySlot].chunk, chunkSize, voxelSize);

    return slots[emptySlot].chunk;
}

void UpdateChunkLoading(ChunkSlot* slots, int maxSlots, vec3 playerPos, float voxelSize, int chunkSize, int renderDist) {
    float chunkWorldSize = chunkSize * voxelSize;

    int playerChunkX = (int)floorf(playerPos.x / chunkWorldSize);
    int playerChunkZ = (int)floorf(playerPos.z / chunkWorldSize);

    int halfDist = renderDist / 2;
    for (int cx = playerChunkX - halfDist; cx <= playerChunkX + halfDist; cx++) {
        for (int cz = playerChunkZ - halfDist; cz <= playerChunkZ + halfDist; cz++) {
            GetOrCreateChunk(slots, maxSlots, cx, cz, voxelSize, chunkSize);
        }
    }

    for (int i = 0; i < maxSlots; i++) {
        if (slots[i].loaded) {
            int dx = abs(slots[i].chunkX - playerChunkX);
            int dz = abs(slots[i].chunkZ - playerChunkZ);

            if (dx > halfDist + 1 || dz > halfDist + 1) {
                extern void FreeChunkMesh(Chunk* chunk);
                FreeChunkMesh(slots[i].chunk);
                for (int x = 0; x < chunkSize; x++)
                    for (int y = 0; y < chunkSize; y++)
                        for (int z = 0; z < chunkSize; z++)
                            if (slots[i].chunk->blocks[x][y][z])
                                free(slots[i].chunk->blocks[x][y][z]);
                free(slots[i].chunk);
                slots[i].chunk = NULL;
                slots[i].loaded = false;
            }
        }
    }
}

void FreeAllChunks(ChunkSlot* slots, int maxSlots, int chunkSize) {
    extern void FreeChunkMesh(Chunk* chunk);
    for (int i = 0; i < maxSlots; i++) {
        if (slots[i].loaded && slots[i].chunk) {
            FreeChunkMesh(slots[i].chunk);
            for (int x = 0; x < chunkSize; x++)
                for (int y = 0; y < chunkSize; y++)
                    for (int z = 0; z < chunkSize; z++)
                        if (slots[i].chunk->blocks[x][y][z])
                            free(slots[i].chunk->blocks[x][y][z]);
            free(slots[i].chunk);
            slots[i].chunk = NULL;
            slots[i].loaded = false;
        }
    }
}
