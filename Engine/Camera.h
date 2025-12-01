#ifndef CAMERA_H
#define CAMERA_H

#include "utils/MathUtil.h"

typedef struct {
    vec3 pos;
    float yaw;
    float pitch;
} camera;

void InitCamera(camera* cam, vec3 pos);
void ProcessInput(camera* cam, float deltaTime);
void ApplyCameraView(camera* cam);

#endif
