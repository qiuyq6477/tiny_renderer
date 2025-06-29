#include "vector.h"
#include <math.h>

// TODO: Implementation of all vector functions

// 2D 向量操作
vec2_t vec2_add(vec2_t a, vec2_t b) {
    return (vec2_t){a.x + b.x, a.y + b.y};
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
    return (vec2_t){a.x - b.x, a.y - b.y};
}

vec2_t vec2_scale(vec2_t v, float s) {
    return (vec2_t){v.x * s, v.y * s};
}

float vec2_dot(vec2_t a, vec2_t b) {
    return a.x * b.x + a.y * b.y;
}

float vec2_length(vec2_t v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

vec2_t vec2_normalize(vec2_t v) {
    float len = vec2_length(v);
    if (len > 0) return vec2_scale(v, 1.0f / len);
    return v;
}

// 3D 向量操作
vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3_t vec3_scale(vec3_t v, float s) {
    return (vec3_t){v.x * s, v.y * s, v.z * s};
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return (vec3_t){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float vec3_length(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_normalize(vec3_t v) {
    float len = vec3_length(v);
    if (len > 0) return vec3_scale(v, 1.0f / len);
    return v;
}

vec3_t vec3_neg(vec3_t v){
    return (vec3_t){
        -v.x,
        -v.y,
        -v.z
    };
}