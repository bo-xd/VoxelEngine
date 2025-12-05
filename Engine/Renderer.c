#include "Renderer.h"
#include "World/Block.h"

static bool VoxelInFrustum(vec3 pos, mat4 view, mat4 proj, float voxelSize) {
    mat4 vp = Mat4Multiply(proj, view);
    vec4 corners[8] = {
        {pos.x-voxelSize/2,pos.y-voxelSize/2,pos.z-voxelSize/2,1},
        {pos.x+voxelSize/2,pos.y-voxelSize/2,pos.z-voxelSize/2,1},
        {pos.x-voxelSize/2,pos.y+voxelSize/2,pos.z-voxelSize/2,1},
        {pos.x+voxelSize/2,pos.y+voxelSize/2,pos.z-voxelSize/2,1},
        {pos.x-voxelSize/2,pos.y-voxelSize/2,pos.z+voxelSize/2,1},
        {pos.x+voxelSize/2,pos.y-voxelSize/2,pos.z+voxelSize/2,1},
        {pos.x-voxelSize/2,pos.y+voxelSize/2,pos.z+voxelSize/2,1},
        {pos.x+voxelSize/2,pos.y+voxelSize/2,pos.z+voxelSize/2,1}
    };
    for (int i = 0; i < 8; i++) {
        vec4 clip = Mat4MultiplyVec4(vp, corners[i]);
        if (clip.x >= -clip.w && clip.x <= clip.w &&
            clip.y >= -clip.w && clip.y <= clip.w &&
            clip.z >= -clip.w && clip.z <= clip.w)
            return true;
    }
    return false;
}

VoxelMesh CreateVoxelMesh(float size) {
    VoxelMesh mesh = {0};
    float s = size * 0.5f;
    float vertices[] = {
        -s,-s, s,  0,0,1,   s,-s, s,  0,0,1,   s, s, s,  0,0,1,
        -s,-s, s,  0,0,1,   s, s, s,  0,0,1,   -s, s, s,  0,0,1,
         s,-s,-s, 0,0,-1,  -s,-s,-s,0,0,-1, -s, s,-s,0,0,-1,
         s,-s,-s, 0,0,-1,  -s, s,-s,0,0,-1,  s, s,-s,0,0,-1,
        -s,-s,-s,-1,0,0,  -s,-s, s,-1,0,0,  -s, s, s,-1,0,0,
        -s,-s,-s,-1,0,0,  -s, s, s,-1,0,0,  -s, s,-s,-1,0,0,
         s,-s, s,1,0,0,   s,-s,-s,1,0,0,   s, s,-s,1,0,0,
         s,-s, s,1,0,0,   s, s,-s,1,0,0,   s, s, s,1,0,0,
        -s, s, s,0,1,0,   s, s, s,0,1,0,   s, s,-s,0,1,0,
        -s, s, s,0,1,0,   s, s,-s,0,1,0,   -s, s,-s,0,1,0,
        -s,-s,-s,0,-1,0,  s,-s,-s,0,-1,0,  s,-s, s,0,-1,0,
        -s,-s,-s,0,-1,0,  s,-s, s,0,-1,0,  -s,-s, s,0,-1,0
    };
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    mesh.vertexCount = 36;
    glBindVertexArray(0);
    return mesh;
}

void Shader_SetMat4(shader* s, const char* name, const mat4* mat) {
    GLint loc = glGetUniformLocation(s->id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat->m);
}

void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection, vec3 color) {
    Shader_Use(s);
    mat4 model = Mat4Identity();
    model = Mat4Translate(model, pos);
    Shader_SetMat4(s, "model", &model);
    Shader_SetMat4(s, "view", &view);
    Shader_SetMat4(s, "projection", &projection);
    GLint colorLoc = glGetUniformLocation(s->id, "blockColor");
    glUniform3f(colorLoc, color.x, color.y, color.z);
    glBindVertexArray(voxel->VAO);
    glDrawArrays(GL_TRIANGLES, 0, voxel->vertexCount);
    glBindVertexArray(0);
}

void DrawChunk(const Chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection, int size, vec3 camPos, float maxDist) {
    float voxelSize = 0.2f;
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            for (int z = 0; z < size; z++) {
                Block* b = c->blocks[x][y][z];
                if (!b || !b->active) continue;
                vec3 diff = Vec3Subtract(b->position, camPos);
                float dist2 = Vec3LengthSquared(diff);
                if (dist2 > maxDist * maxDist) continue;
                if (!VoxelInFrustum(b->position, view, projection, voxelSize)) continue;
                vec3 color = b->color;
                DrawVoxel(voxel, s, b->position, view, projection, color);
            }
        }
    }

}

Skybox CreateSkybox() {
    Skybox sb = {0};
    float skyboxVertices[] = {
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        // Back face
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        // Left face
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // Right face
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
        // Top face
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // Bottom face
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f
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
