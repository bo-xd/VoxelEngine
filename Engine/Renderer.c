#include "Renderer.h"
#include <stdlib.h>
#include <math.h>

static bool VoxelInFrustum(vec3 pos, mat4 view, mat4 proj, float voxelSize){
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
    for(int i=0;i<8;i++){
        vec4 clip=Mat4MultiplyVec4(vp,corners[i]);
        if(clip.x>=-clip.w&&clip.x<=clip.w&&clip.y>=-clip.w&&clip.y<=clip.w&&clip.z>=-clip.w&&clip.z<=clip.w)
            return true;
    }
    return false;
}

bool IsFaceVisible(Chunk* c, int x, int y, int z, int dx, int dy, int dz) {
    int nx = x + dx;
    int ny = y + dy;
    int nz = z + dz;
    if (nx < 0 || nx >= 32 || ny < 0 || ny >= 32 || nz < 0 || nz >= 32)
        return true;
    Block* n = c->blocks[nx][ny][nz];
    if (!n) return true;
    return !n->active;
}

VoxelMesh CreateVoxelMesh(float size) {
    VoxelMesh mesh = {0};
    float s = size * 0.5f;
    float vertices[] = {
        -s,-s, s,0,0,1, s,-s, s,0,0,1, s, s, s,0,0,1,
        -s,-s, s,0,0,1, s, s, s,0,0,1, -s, s, s,0,0,1,
        s,-s,-s,0,0,-1,-s,-s,-s,0,0,-1,-s, s,-s,0,0,-1,
        s,-s,-s,0,0,-1,-s, s,-s,0,0,-1,s, s,-s,0,0,-1,
        -s,-s,-s,-1,0,0,-s,-s, s,-1,0,0,-s, s, s,-1,0,0,
        -s,-s,-s,-1,0,0,-s, s, s,-1,0,0,-s, s,-s,-1,0,0,
        s,-s, s,1,0,0,s,-s,-s,1,0,0,s, s,-s,1,0,0,
        s,-s, s,1,0,0,s, s,-s,1,0,0,s, s, s,1,0,0,
        -s, s, s,0,1,0,s, s, s,0,1,0,s, s,-s,0,1,0,
        -s, s, s,0,1,0,s, s,-s,0,1,0,-s, s,-s,0,1,0,
        -s,-s,-s,0,-1,0,s,-s,-s,0,-1,0,s,-s, s,0,-1,0,
        -s,-s,-s,0,-1,0,s,-s, s,0,-1,0,-s,-s, s,0,-1,0
    };
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    mesh.vertexCount = 36;
    glBindVertexArray(0);
    return mesh;
}


void DrawVoxel(const VoxelMesh* voxel, shader* s, vec3 pos, mat4 view, mat4 projection, vec3 color) {
    Shader_Use(s);
    mat4 model = Mat4Translate(Mat4Identity(), pos);
    Shader_SetMat4(s, "model", &model);
    Shader_SetMat4(s, "view", &view);
    Shader_SetMat4(s, "projection", &projection);
    GLint colorLoc = glGetUniformLocation(s->id, "blockColor");
    glUniform3f(colorLoc, color.x, color.y, color.z);
    glBindVertexArray(voxel->VAO);
    glDrawArrays(GL_TRIANGLES,0,voxel->vertexCount);
    glBindVertexArray(0);
}

void DrawChunk(const Chunk* c, const VoxelMesh* voxel, shader* s, mat4 view, mat4 projection, int size, vec3 camPos, float maxDist) {
    if (!c) return;
    vec3 chunkCenter = {c->position.x+size*0.1f, c->position.y+size*0.1f, c->position.z+size*0.1f};
    float chunkExtent = sqrtf(3.0f)*0.1f*size;
    vec3 diff = Vec3Subtract(chunkCenter, camPos);
    float dist2 = Vec3LengthSquared(diff);
    if (dist2 > (maxDist+chunkExtent)*(maxDist+chunkExtent)) return;
    if (!VoxelInFrustum(chunkCenter, view, projection, chunkExtent)) return;
    if (c->meshVAO==0||c->meshVertexCount==0) return;
    Shader_Use(s);
    Shader_SetMat4(s,"model",&(mat4){.m={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}});
    Shader_SetMat4(s,"view",&view);
    Shader_SetMat4(s,"projection",&projection);
    glBindVertexArray(c->meshVAO);
    glDrawArrays(GL_TRIANGLES,0,c->meshVertexCount);
    glBindVertexArray(0);
}

SkyDome CreateSkyDome(int slices, int stacks, vec3 topColor, vec3 bottomColor) {
    SkyDome dome = {0};
    dome.topColor = topColor;
    dome.bottomColor = bottomColor;

    int vertCount = slices * stacks * 6;
    float* data = malloc(vertCount * 6 * sizeof(float));
    if(!data) return dome;

    int idx = 0;
    for(int i = 0; i < stacks; i++) {
        float phi0 = (float)i / stacks * M_PI;
        float phi1 = (float)(i+1) / stacks * M_PI;

        float y0 = cosf(phi0);
        float y1 = cosf(phi1);
        float r0 = sinf(phi0);
        float r1 = sinf(phi1);

        for(int j = 0; j < slices; j++) {
            float theta0 = (float)j / slices * 2*M_PI;
            float theta1 = (float)(j+1) / slices * 2*M_PI;

            float x00 = r0*cosf(theta0), z00 = r0*sinf(theta0);
            float x01 = r0*cosf(theta1), z01 = r0*sinf(theta1);
            float x10 = r1*cosf(theta0), z10 = r1*sinf(theta0);
            float x11 = r1*cosf(theta1), z11 = r1*sinf(theta1);

            float t0 = (y0 + 1.0f) * 0.5f; // map y [-1,1] -> [0,1]
            float t1 = (y1 + 1.0f) * 0.5f;

            vec3 color0 = {
                bottomColor.x*(1.0f-t0)+topColor.x*t0,
                bottomColor.y*(1.0f-t0)+topColor.y*t0,
                bottomColor.z*(1.0f-t0)+topColor.z*t0
            };
            vec3 color1 = {
                bottomColor.x*(1.0f-t1)+topColor.x*t1,
                bottomColor.y*(1.0f-t1)+topColor.y*t1,
                bottomColor.z*(1.0f-t1)+topColor.z*t1
            };

            // Triangle 1
            data[idx++] = x10; data[idx++] = y1; data[idx++] = z10;
            data[idx++] = color1.x; data[idx++] = color1.y; data[idx++] = color1.z;

            data[idx++] = x00; data[idx++] = y0; data[idx++] = z00;
            data[idx++] = color0.x; data[idx++] = color0.y; data[idx++] = color0.z;

            data[idx++] = x01; data[idx++] = y0; data[idx++] = z01;
            data[idx++] = color0.x; data[idx++] = color0.y; data[idx++] = color0.z;

            // Triangle 2
            data[idx++] = x10; data[idx++] = y1; data[idx++] = z10;
            data[idx++] = color1.x; data[idx++] = color1.y; data[idx++] = color1.z;

            data[idx++] = x01; data[idx++] = y0; data[idx++] = z01;
            data[idx++] = color0.x; data[idx++] = color0.y; data[idx++] = color0.z;

            data[idx++] = x11; data[idx++] = y1; data[idx++] = z11;
            data[idx++] = color1.x; data[idx++] = color1.y; data[idx++] = color1.z;
        }
    }

    dome.vertexCount = idx / 6;
    glGenVertexArrays(1, &dome.VAO);
    glGenBuffers(1, &dome.VBO);
    glBindVertexArray(dome.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, dome.VBO);
    glBufferData(GL_ARRAY_BUFFER, idx * sizeof(float), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    free(data);

    return dome;
}


void DrawSkyDome(SkyDome* dome, shader* s, mat4 view, mat4 projection) {
    glDepthMask(GL_FALSE);
    Shader_Use(s);

    mat4 viewNoTrans = view;
    viewNoTrans.m[12] = 0;
    viewNoTrans.m[13] = 0;
    viewNoTrans.m[14] = 0;
    Shader_SetMat4(s, "view", &viewNoTrans);
    Shader_SetMat4(s, "projection", &projection);

    glBindVertexArray(dome->VAO);
    glDrawArrays(GL_TRIANGLES, 0, dome->vertexCount);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
}



void FreeSkyDome(SkyDome* dome){
    if(!dome) return;
    if(dome->VAO){glDeleteVertexArrays(1,&dome->VAO);dome->VAO=0;}
    if(dome->VBO){glDeleteBuffers(1,&dome->VBO);dome->VBO=0;}
    dome->vertexCount=0;
}


void BuildChunkMesh(Chunk* c,int size,float voxelSize) {
    if(!c) return;
    if(c->meshVAO){glDeleteVertexArrays(1,&c->meshVAO);c->meshVAO=0;}
    if(c->meshVBO){glDeleteBuffers(1,&c->meshVBO);c->meshVBO=0;}
    c->meshVertexCount=0;
    float* data=NULL;
    size_t capacity=0,count=0;
    float s=voxelSize*0.5f;
    const int faces[6][3]={{0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
    const float faceVerts[6][6][3]={{
        {-s,-s,s},{s,-s,s},{s,s,s},{-s,-s,s},{s,s,s},{-s,s,s}},
        {{s,-s,-s},{-s,-s,-s},{-s,s,-s},{s,-s,-s},{-s,s,-s},{s,s,-s}},
        {{-s,-s,-s},{-s,-s,s},{-s,s,s},{-s,-s,-s},{-s,s,s},{-s,s,-s}},
        {{s,-s,s},{s,-s,-s},{s,s,-s},{s,-s,s},{s,s,-s},{s,s,s}},
        {{-s,s,s},{s,s,s},{s,s,-s},{-s,s,s},{s,s,-s},{-s,s,-s}},
        {{-s,-s,-s},{s,-s,-s},{s,-s,s},{-s,-s,-s},{s,-s,s},{-s,-s,s}}
    };
    const float faceNormals[6][3]={{0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
    for(int x=0;x<size;x++) for(int y=0;y<size;y++) for(int z=0;z<size;z++){
        Block* b=c->blocks[x][y][z];
        if(!b||!b->active) continue;
        for(int f=0;f<6;f++){
            int dx=faces[f][0],dy=faces[f][1],dz=faces[f][2];
            if(!IsFaceVisible(c,x,y,z,dx,dy,dz)) continue;
            size_t need=count+6*9;
            if(need>capacity){
                size_t newcap=capacity?capacity*2:1024;
                while(newcap<need) newcap*=2;
                float* tmp=(float*)realloc(data,newcap*sizeof(float));
                if(!tmp){free(data);return;}
                data=tmp;capacity=newcap;
            }
            for(int vi=0;vi<6;vi++){
                float vx=faceVerts[f][vi][0]+b->position.x;
                float vy=faceVerts[f][vi][1]+b->position.y;
                float vz=faceVerts[f][vi][2]+b->position.z;
                float nx=faceNormals[f][0],ny=faceNormals[f][1],nz=faceNormals[f][2];
                float ao=1.0f;
                int nxp=x+(dx>0?1:dx<0?-1:0);
                int nyp=y+(dy>0?1:dy<0?-1:0);
                int nzp=z+(dz>0?1:dz<0?-1:0);
                if(nxp>=0&&nxp<size&&nyp>=0&&nyp<size&&nzp>=0&&nzp<size){
                    Block* nb=c->blocks[nxp][nyp][nzp];
                    if(nb&&nb->active) ao=0.7f;
                }
                data[count++]=vx;data[count++]=vy;data[count++]=vz;
                data[count++]=nx;data[count++]=ny;data[count++]=nz;
                data[count++]=b->color.x*ao;
                data[count++]=b->color.y*ao;
                data[count++]=b->color.z*ao;
            }
        }
    }
    if(count==0){free(data);return;}
    glGenVertexArrays(1,&c->meshVAO);
    glGenBuffers(1,&c->meshVBO);
    glBindVertexArray(c->meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER,c->meshVBO);
    glBufferData(GL_ARRAY_BUFFER,count*sizeof(float),data,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    c->meshVertexCount=count/9;
    free(data);
}

void FreeChunkMesh(Chunk* c){
    if(!c) return;
    if(c->meshVAO){glDeleteVertexArrays(1,&c->meshVAO);c->meshVAO=0;}
    if(c->meshVBO){glDeleteBuffers(1,&c->meshVBO);c->meshVBO=0;}
    c->meshVertexCount=0;
}
