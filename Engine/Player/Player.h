#ifndef PLAYER_H
#define PLAYER_H

#include "../Camera.h"
#include "../World/Block.h"
#include "../utils/MathUtil.h"
#include <stdbool.h>

typedef struct {
    vec3 position;
    vec3 velocity;
    camera cam;

    float width;
    float height;
    float eyeHeight;

    bool onGround;
    bool jumping;
} Player;

void InitPlayer(Player* player, vec3 startPos);
void UpdatePlayer(Player* player, float deltaTime, Chunk** chunks, int chunkCount, int chunkSize, float voxelSize);
void ProcessPlayerInput(Player* player, float deltaTime);
void ProcessPlayerMouseMovement(Player* player, float xrel, float yrel);
bool CheckCollision(vec3 pos, float width, float height, Chunk** chunks, int chunkCount, int chunkSize, float voxelSize);
vec3 GetPlayerCameraPosition(Player* player);

#endif
