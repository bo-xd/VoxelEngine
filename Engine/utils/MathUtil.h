#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <endian.h>
#include <math.h>

typedef struct {
    float x,y,z;
} vec3;

typedef struct {
    float m[16];
} mat4;

static vec3 Vec3Subtract(vec3 a, vec3 b) {
    vec3 r = {a.x - b.x, a.y - b.y, a.z - b.z};
    return r;
}

static vec3 Vec3Normalize(vec3 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    vec3 r = {v.x/len, v.y/len, v.z/len};
    return r;
}

static vec3 Vec3Cross(vec3 a, vec3 b) {
    vec3 r = {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
    return r;
}

static mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 f = Vec3Normalize(Vec3Subtract(center, eye));
    vec3 s = Vec3Normalize(Vec3Cross(f, up));
    vec3 u = Vec3Cross(s, f);

    mat4 result = {{
        s.x,  u.x, -f.x, 0.0f,
        s.y,  u.y, -f.y, 0.0f,
        s.z,  u.z, -f.z, 0.0f,
       - (s.x*eye.x + s.y*eye.y + s.z*eye.z),
       - (u.x*eye.x + u.y*eye.y + u.z*eye.z),
         (f.x*eye.x + f.y*eye.y + f.z*eye.z),
        1.0f
    }};

    return result;
}

#endif
