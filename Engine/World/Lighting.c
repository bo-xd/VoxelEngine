#include "Lighting.h"
#include <GL/glew.h>
#include <stdio.h>

void SetDirectionalLightUniforms(const DirectionalLight* light, unsigned int shaderID, vec3 viewPos) {
    char name[64];

    snprintf(name, sizeof(name), "dirLight.direction");
    glUniform3f(glGetUniformLocation(shaderID, name), light->direction.x, light->direction.y, light->direction.z);

    snprintf(name, sizeof(name), "dirLight.color");
    glUniform3f(glGetUniformLocation(shaderID, name), light->color.x, light->color.y, light->color.z);

    snprintf(name, sizeof(name), "dirLight.ambient");
    glUniform1f(glGetUniformLocation(shaderID, name), light->ambient);

    snprintf(name, sizeof(name), "dirLight.diffuse");
    glUniform1f(glGetUniformLocation(shaderID, name), light->diffuse);

    snprintf(name, sizeof(name), "dirLight.specular");
    glUniform1f(glGetUniformLocation(shaderID, name), light->specular);

    glUniform3f(glGetUniformLocation(shaderID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
}
