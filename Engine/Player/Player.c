#include "Player.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <math.h>

#define GRAVITY -20.0f
#define JUMP_STRENGTH 8.0f
#define MOVE_SPEED 4.3f
#define PLAYER_WIDTH 0.6f
#define PLAYER_HEIGHT 1.8f
#define PLAYER_EYE_HEIGHT 1.62f

void InitPlayer(Player* player, vec3 startPos) {
    player->position = startPos;
    player->velocity = (vec3){0.0f, 0.0f, 0.0f};
    player->width = PLAYER_WIDTH;
    player->height = PLAYER_HEIGHT;
    player->eyeHeight = PLAYER_EYE_HEIGHT;
    player->onGround = false;
    player->jumping = false;

    vec3 camPos = {startPos.x, startPos.y + player->eyeHeight, startPos.z};
    InitCamera(&player->cam, camPos);
}

bool IsBlockSolid(Chunk** chunks, int chunkCount, vec3 worldPos, int chunkSize, float voxelSize) {
    for (int i = 0; i < chunkCount; i++) {
        Chunk* c = chunks[i];
        if (!c) continue;

        float relX = worldPos.x - c->position.x;
        float relY = worldPos.y - c->position.y;
        float relZ = worldPos.z - c->position.z;

        int bx = (int)(relX / voxelSize);
        int by = (int)(relY / voxelSize);
        int bz = (int)(relZ / voxelSize);

        if (bx >= 0 && bx < chunkSize && by >= 0 && by < chunkSize && bz >= 0 && bz < chunkSize) {
            Block* b = c->blocks[bx][by][bz];
            if (b && b->active) return true;
        }
    }
    return false;
}

bool CheckCollision(vec3 pos, float width, float height, Chunk** chunks, int chunkCount, int chunkSize, float voxelSize) {
    float halfWidth = width * 0.5f;

    vec3 checkPoints[8] = {
        {pos.x - halfWidth, pos.y, pos.z - halfWidth},
        {pos.x + halfWidth, pos.y, pos.z - halfWidth},
        {pos.x - halfWidth, pos.y, pos.z + halfWidth},
        {pos.x + halfWidth, pos.y, pos.z + halfWidth},
        {pos.x - halfWidth, pos.y + height, pos.z - halfWidth},
        {pos.x + halfWidth, pos.y + height, pos.z - halfWidth},
        {pos.x - halfWidth, pos.y + height, pos.z + halfWidth},
        {pos.x + halfWidth, pos.y + height, pos.z + halfWidth}
    };

    for (int i = 0; i < 8; i++) {
        if (IsBlockSolid(chunks, chunkCount, checkPoints[i], chunkSize, voxelSize)) {
            return true;
        }
    }
    return false;
}

void ProcessPlayerInput(Player* player, float deltaTime) {
    const bool* state = SDL_GetKeyboardState(NULL);
    vec3 front = CameraFront(&player->cam);
    vec3 right = CameraRight(&player->cam);

    front.y = 0.0f;
    front = Vec3Normalize(front);
    right.y = 0.0f;
    right = Vec3Normalize(right);

    vec3 moveDir = {0.0f, 0.0f, 0.0f};

    if (state[SDL_SCANCODE_W]) {
        moveDir.x += front.x;
        moveDir.z += front.z;
    }
    if (state[SDL_SCANCODE_S]) {
        moveDir.x -= front.x;
        moveDir.z -= front.z;
    }
    if (state[SDL_SCANCODE_A]) {
        moveDir.x -= right.x;
        moveDir.z -= right.z;
    }
    if (state[SDL_SCANCODE_D]) {
        moveDir.x += right.x;
        moveDir.z += right.z;
    }

    float len = sqrtf(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
    if (len > 0.0f) {
        moveDir.x /= len;
        moveDir.z /= len;
    }

    player->velocity.x = moveDir.x * MOVE_SPEED;
    player->velocity.z = moveDir.z * MOVE_SPEED;

    if (state[SDL_SCANCODE_SPACE] && player->onGround && !player->jumping) {
        player->velocity.y = JUMP_STRENGTH;
        player->jumping = true;
        player->onGround = false;
    }

    if (!state[SDL_SCANCODE_SPACE]) {
        player->jumping = false;
    }
}

void ProcessPlayerMouseMovement(Player* player, float xrel, float yrel) {
    ProcessMouseMovement(&player->cam, xrel, yrel);
}

void UpdatePlayer(Player* player, float deltaTime, Chunk** chunks, int chunkCount, int chunkSize, float voxelSize) {
    player->velocity.y += GRAVITY * deltaTime;

    vec3 newPos = player->position;

    newPos.x += player->velocity.x * deltaTime;
    if (CheckCollision(newPos, player->width, player->height, chunks, chunkCount, chunkSize, voxelSize)) {
        newPos.x = player->position.x;
        player->velocity.x = 0.0f;
    }

    newPos.z += player->velocity.z * deltaTime;
    if (CheckCollision(newPos, player->width, player->height, chunks, chunkCount, chunkSize, voxelSize)) {
        newPos.z = player->position.z;
        player->velocity.z = 0.0f;
    }

    newPos.y += player->velocity.y * deltaTime;
    if (CheckCollision(newPos, player->width, player->height, chunks, chunkCount, chunkSize, voxelSize)) {
        if (player->velocity.y < 0.0f) {
            player->onGround = true;
        }
        newPos.y = player->position.y;
        player->velocity.y = 0.0f;
    } else {
        player->onGround = false;
    }

    player->position = newPos;
    player->cam.pos.x = player->position.x;
    player->cam.pos.y = player->position.y + player->eyeHeight;
    player->cam.pos.z = player->position.z;
}

vec3 GetPlayerCameraPosition(Player* player) {
    return player->cam.pos;
}
