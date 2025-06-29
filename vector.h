#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

// 2D vector functions
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_scale(vec2_t v, float s);
float  vec2_dot(vec2_t a, vec2_t b);
float  vec2_length(vec2_t v);
vec2_t vec2_normalize(vec2_t v);

// 3D vector functions
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_scale(vec3_t v, float s);
float  vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float  vec3_length(vec3_t v);
vec3_t vec3_normalize(vec3_t v);
vec3_t vec3_neg(vec3_t v);
// TODO: Add functions to manipulate vectors 2D and 3D
// ...

#endif
