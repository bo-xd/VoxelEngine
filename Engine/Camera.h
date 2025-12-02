#ifndef CAMERA_H
#define CAMERA_H

#include "utils/MathUtil.h"

typedef struct {
    vec3 pos;
    float yaw, pitch;
} camera;

void InitCamera(camera* cam, vec3 pos);
void ProcessInput(camera* cam, float deltaTime);
void ProcessMouseMovement(camera* cam, float xrel, float yrel);
void ApplyCameraView(camera* cam);
vec3 CameraFront(camera* cam);

#endif
