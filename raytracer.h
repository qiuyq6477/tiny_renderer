#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <stdint.h>
#include <stdbool.h>
#include "vector.h"

// 球体结构体
typedef struct {
    vec3_t center;
    float radius;
    uint32_t color;
    float specular;
} sphere_t;

// 相交结果结构体
typedef struct {
    float t1, t2;
} intersection_result_t;

// 光源类型
enum light_type {
    LIGHT_AMBIENT,
    LIGHT_POINT,
    LIGHT_DIRECTIONAL
};

// 光源结构体
typedef struct {
    enum light_type ltype;
    float intensity;
    vec3_t position;
} light_t;


// 场景参数
#define VIEWPORT_SIZE 1.0f
#define PROJECTION_PLANE_Z 1.0f
#define BACKGROUND_COLOR 0xFFFFFFFF
#define NUM_SPHERES 4
#define NUM_LIGHTS 3

// 全局变量声明
extern sphere_t spheres[NUM_SPHERES];
extern light_t lights[NUM_LIGHTS];
extern vec3_t camera_position;

// 向量运算函数
float dot_product(vec3_t v1, vec3_t v2);
vec3_t subtract(vec3_t v1, vec3_t v2);
vec3_t normalize(vec3_t v);

// 坐标转换函数
vec3_t canvas_to_viewport(int x, int y);

// 光线追踪函数
intersection_result_t intersect_ray_sphere(vec3_t origin, vec3_t direction, sphere_t sphere);
uint32_t trace_ray(vec3_t origin, vec3_t direction, float min_t, float max_t);

// 场景初始化函数
void init_scene(void);

#endif // RAYTRACER_H 