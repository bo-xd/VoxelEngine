#ifndef LIGHTING_H
#define LIGHTING_H

#include "../utils/MathUtil.h"

typedef struct {
    vec3 direction;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
} DirectionalLight;

void SetDirectionalLightUniforms(const DirectionalLight* light, unsigned int shaderID, vec3 viewPos);

#endif
