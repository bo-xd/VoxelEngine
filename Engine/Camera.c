#include "Camera.h"
#include "utils/MathUtil.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <GL/gl.h>
#include <math.h>

void InitCamera(camera* cam, vec3 pos) {
    cam->pos = pos;
    cam->yaw = 90.0f;
    cam->pitch = 0.0f;
}

vec3 CameraFront(camera* cam) {
    vec3 front;
    front.x = cosf(cam->yaw * M_PI / 180.0f) * cosf(cam->pitch * M_PI / 180.0f);
    front.y = sinf(cam->pitch * M_PI / 180.0f);
    front.z = sinf(cam->yaw * M_PI / 180.0f) * cosf(cam->pitch * M_PI / 180.0f);
    return Vec3Normalize(front);
}

vec3 CameraRight(camera* cam) {
    vec3 front = CameraFront(cam);
    vec3 up = { 0.0f, 1.0f, 0.0f };
    return Vec3Normalize(Vec3Cross(front, up));
}

void ProcessInput(camera* cam, float deltaTime) {
    const float speed = 6.0f * deltaTime;
    const bool* state = SDL_GetKeyboardState(NULL);

    vec3 front = CameraFront(cam);
    vec3 right = CameraRight(cam);

    if (state[SDL_SCANCODE_W]) {
        cam->pos.x += front.x * speed;
        cam->pos.y += front.y * speed;
        cam->pos.z += front.z * speed;
    }
    if (state[SDL_SCANCODE_S]) {
        cam->pos.x -= front.x * speed;
        cam->pos.y -= front.y * speed;
        cam->pos.z -= front.z * speed;
    }
    if (state[SDL_SCANCODE_A]) {
        cam->pos.x -= right.x * speed;
        cam->pos.y -= right.y * speed;
        cam->pos.z -= right.z * speed;
    }
    if (state[SDL_SCANCODE_D]) {
        cam->pos.x += right.x * speed;
        cam->pos.y += right.y * speed;
        cam->pos.z += right.z * speed;
    }


    if (state[SDL_SCANCODE_E]) cam->pos.y += speed;
    if (state[SDL_SCANCODE_Q]) cam->pos.y -= speed;
}


void ProcessMouseMovement(camera* cam, float xrel, float yrel) {
    const float sensitivity = 0.1f;

    cam->yaw   += xrel * sensitivity;
    cam->pitch -= yrel * sensitivity;

    if (cam->pitch > 89.0f)  cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;
}

void ApplyCameraView(camera* cam) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    vec3 front = {
        cosf(cam->yaw * M_PI / 180.0f) * cosf(cam->pitch * M_PI / 180.0f),
        sinf(cam->pitch * M_PI / 180.0f),
        sinf(cam->yaw * M_PI / 180.0f) * cosf(cam->pitch * M_PI / 180.0f)
    };
    vec3 target = { cam->pos.x + front.x, cam->pos.y + front.y, cam->pos.z + front.z };
    vec3 up = {0.0f, 1.0f, 0.0f};

    mat4 view = LookAt(cam->pos, target, up);
    glLoadMatrixf(view.m);
}
