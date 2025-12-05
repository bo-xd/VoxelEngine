#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <math.h>

typedef struct { float x, y, z; } vec3;
typedef struct { float m[16]; } mat4;
typedef struct { float x, y, z, w; } vec4;

static vec3 Vec3Add(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static vec3 Vec3Subtract(vec3 a, vec3 b) { return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static vec3 Vec3Scale(vec3 v, float s) { return (vec3){v.x*s, v.y*s, v.z*s}; }
static float Vec3Length(vec3 v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
static float Vec3LengthSquared(vec3 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
static vec3 Vec3Normalize(vec3 v) {
    float len = Vec3Length(v);
    if (len == 0.0f) return (vec3){0,0,0};
    return Vec3Scale(v, 1.0f/len);
}
static vec3 Vec3Cross(vec3 a, vec3 b) { return (vec3){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }
static float Vec3Dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

static mat4 Mat4Identity() {
    return (mat4){{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    }};
}

static mat4 Mat4Translate(mat4 m, vec3 v) {
    mat4 r = m;
    r.m[12] += v.x;
    r.m[13] += v.y;
    r.m[14] += v.z;
    return r;
}

static mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 f = Vec3Normalize(Vec3Subtract(center, eye));
    vec3 s = Vec3Normalize(Vec3Cross(f, up));
    vec3 u = Vec3Cross(s, f);

    return (mat4){{
        s.x,  u.x, -f.x, 0.0f,
        s.y,  u.y, -f.y, 0.0f,
        s.z,  u.z, -f.z, 0.0f,
       -Vec3Dot(s, eye),
       -Vec3Dot(u, eye),
        Vec3Dot(f, eye),
        1.0f
    }};
}

static mat4 Perspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov * (M_PI/180.0f) * 0.5f);
    return (mat4){{
        f/aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far+near)/(near-far), -1,
        0, 0, (2*far*near)/(near-far), 0
    }};
}

static vec4 Mat4MultiplyVec4(mat4 m, vec4 v) {
    return (vec4){
        m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z + m.m[12]*v.w,
        m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z + m.m[13]*v.w,
        m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14]*v.w,
        m.m[3]*v.x + m.m[7]*v.y + m.m[11]*v.z + m.m[15]*v.w
    };
}

static mat4 Mat4Multiply(mat4 a, mat4 b) {
    mat4 r = {0};
    for(int row=0; row<4; row++)
        for(int col=0; col<4; col++)
            for(int k=0; k<4; k++)
                r.m[row + col*4] += a.m[row + k*4]*b.m[k + col*4];
    return r;
}

#endif
