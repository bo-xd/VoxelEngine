#include "Renderer.h"
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

chunk* CreateChunk(vec3 pos, int size) {
    chunk* c = (chunk*)malloc(sizeof(chunk));
    if (!c) return NULL;

    c->position = pos;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < 1; y++) {
            for (int z = 0; z < size; z++) {
                c->voxels[x][y][z] = 1;
            }
        }
    }

    for (int x = 0; x < size; x++)
        for (int y = 1; y < size; y++)
            for (int z = 0; z < size; z++)
                c->voxels[x][y][z] = 0;

    return c;
}

void DrawChunk(const chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection, int size) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            for (int z = 0; z < size; z++) {
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


//skybox

Skybox CreateSkybox() {
    Skybox sb = {0};
    float skyboxVertices[] = {
        // pos
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // Back
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Left
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        // Right
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Top
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // Bottom
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &sb.VAO);
    glGenBuffers(1, &sb.VBO);
    glBindVertexArray(sb.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sb.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return sb;
}

void DrawSkybox(Skybox* sb, shader* s, GLuint texture, mat4 view, mat4 projection) {
    glDepthMask(GL_FALSE);
    Shader_Use(s);

    mat4 viewNoTrans = view;
    viewNoTrans.m[12] = 0;
    viewNoTrans.m[13] = 0;
    viewNoTrans.m[14] = 0;

    Shader_SetMat4(s, "view", &viewNoTrans);
    Shader_SetMat4(s, "projection", &projection);

    glBindVertexArray(sb->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}
