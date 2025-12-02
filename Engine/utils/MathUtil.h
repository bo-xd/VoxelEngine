#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <math.h>

typedef struct { float x,y,z; } vec3;
typedef struct { float m[16]; } mat4;

static vec3 Vec3Subtract(vec3 a, vec3 b) { return (vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
static vec3 Vec3Normalize(vec3 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return (vec3){v.x/len, v.y/len, v.z/len};
}
static vec3 Vec3Cross(vec3 a, vec3 b) {
    return (vec3){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

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
       - (s.x*eye.x + s.y*eye.y + s.z*eye.z),
       - (u.x*eye.x + u.y*eye.y + u.z*eye.z),
         (f.x*eye.x + f.y*eye.y + f.z*eye.z),
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

#endif
