#include "Lighting.h"
#include <GL/glew.h>
#include <stdio.h>

void SetDirectionalLightUniforms(const DirectionalLight* light, unsigned int shaderID, vec3 viewPos) {
    glUniform3f(glGetUniformLocation(shaderID, "dirLight.direction"),
                light->direction.x, light->direction.y, light->direction.z);
    glUniform3f(glGetUniformLocation(shaderID, "dirLight.color"),
                light->color.x, light->color.y, light->color.z);
    glUniform1f(glGetUniformLocation(shaderID, "dirLight.ambient"), light->ambient);
    glUniform1f(glGetUniformLocation(shaderID, "dirLight.diffuse"), light->diffuse);
    glUniform1f(glGetUniformLocation(shaderID, "dirLight.specular"), light->specular);
    glUniform3f(glGetUniformLocation(shaderID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
}

float CalculateAO(int side1, int side2, int corner) {
    if (side1 && side2) {
        return 0.0f;
    }
    return 1.0f - (side1 + side2 + corner) * 0.25f;
}
