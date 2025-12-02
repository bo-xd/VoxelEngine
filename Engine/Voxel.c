#include "Voxel.h"
#include <stdlib.h>

VoxelMesh CreateVoxelMesh(float size) {
    VoxelMesh mesh = {0};
    float s = size * 0.5f;

    float vertices[] = {
        // Front
        -s, -s,  s,   s, -s,  s,   s,  s,  s,
        -s, -s,  s,   s,  s,  s,  -s,  s,  s,
        // Back
         s, -s, -s,  -s, -s, -s,  -s,  s, -s,
         s, -s, -s,  -s,  s, -s,   s,  s, -s,
        // Left
        -s, -s, -s,  -s, -s,  s,  -s,  s,  s,
        -s, -s, -s,  -s,  s,  s,  -s,  s, -s,
        // Right
         s, -s,  s,   s, -s, -s,   s,  s, -s,
         s, -s,  s,   s,  s, -s,   s,  s,  s,
        // Top
        -s,  s,  s,   s,  s,  s,   s,  s, -s,
        -s,  s,  s,   s,  s, -s,  -s,  s, -s,
        // Bottom
        -s, -s, -s,   s, -s, -s,   s, -s,  s,
        -s, -s, -s,   s, -s,  s,  -s, -s,  s,
    };

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return mesh;
}

void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection)
{
    Shader_Use(s);

    mat4 model = Mat4Identity();
    model = Mat4Translate(model, pos);

    Shader_SetMat4(s, "model", &model);
    Shader_SetMat4(s, "view", &view);
    Shader_SetMat4(s, "projection", &projection);

    glBindVertexArray(voxel->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Shader_SetMat4(shader* s, const char* name, const mat4* mat) {
    GLint loc = glGetUniformLocation(s->id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat->m);
}

chunk* CreateChunk(vec3 pos) {
    chunk* c = (chunk*)malloc(sizeof(chunk));
    if (!c) return NULL;

    c->position = pos;

    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 1; y++) {
            for (int z = 0; z < 32; z++) {
                c->voxels[x][y][z] = 1;
            }
        }
    }

    for (int x = 0; x < 32; x++)
        for (int y = 1; y < 32; y++)
            for (int z = 0; z < 32; z++)
                c->voxels[x][y][z] = 0;

    return c;
}

void DrawChunk(const chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection) {
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            for (int z = 0; z < 32; z++) {
                if (c->voxels[x][y][z] == 1) {
                    vec3 worldPos = {
                        c->position.x + x,
                        c->position.y + y,
                        c->position.z + z
                    };
                    DrawVoxel(voxel, s, worldPos, view, projection);
                }
            }
        }
    }
}
