#include "raytracer.h"
#include "display.h"
#include <math.h>
#include <stddef.h>

// 全局变量定义
sphere_t spheres[NUM_SPHERES];
vec3_t camera_position = {0, 0, 0};

// 向量点积
float dot_product(vec3_t v1, vec3_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 向量减法
vec3_t subtract(vec3_t v1, vec3_t v2) {
    vec3_t result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return result;
}

// 向量标准化
vec3_t normalize(vec3_t v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 0) {
        vec3_t result = {v.x / length, v.y / length, v.z / length};
        return result;
    }
    return v;
}

// 将2D画布坐标转换为3D视口坐标
vec3_t canvas_to_viewport(int x, int y) {
    vec3_t result = {
        x * VIEWPORT_SIZE / window_width,
        y * VIEWPORT_SIZE / window_height,
        PROJECTION_PLANE_Z
    };
    return result;
}

// 计算光线与球体的相交
intersection_result_t intersect_ray_sphere(vec3_t origin, vec3_t direction, sphere_t sphere) {
    intersection_result_t result = {INFINITY, INFINITY};
    
    vec3_t oc = subtract(origin, sphere.center);
    
    float k1 = dot_product(direction, direction);
    float k2 = 2 * dot_product(oc, direction);
    float k3 = dot_product(oc, oc) - sphere.radius * sphere.radius;
    
    float discriminant = k2 * k2 - 4 * k1 * k3;
    if (discriminant < 0) {
        return result;
    }
    
    result.t1 = (-k2 + sqrt(discriminant)) / (2 * k1);
    result.t2 = (-k2 - sqrt(discriminant)) / (2 * k1);
    
    return result;
}

// 追踪光线
uint32_t trace_ray(vec3_t origin, vec3_t direction, float min_t, float max_t) {
    float closest_t = INFINITY;
    sphere_t* closest_sphere = NULL;
    
    for (int i = 0; i < NUM_SPHERES; i++) {
        intersection_result_t ts = intersect_ray_sphere(origin, direction, spheres[i]);
        
        if (ts.t1 < closest_t && min_t < ts.t1 && ts.t1 < max_t) {
            closest_t = ts.t1;
            closest_sphere = &spheres[i];
        }
        if (ts.t2 < closest_t && min_t < ts.t2 && ts.t2 < max_t) {
            closest_t = ts.t2;
            closest_sphere = &spheres[i];
        }
    }
    
    if (closest_sphere == NULL) {
        return BACKGROUND_COLOR;
    }
    
    return closest_sphere->color;
}

// 初始化场景
void init_scene(void) {
    // 初始化球体
    spheres[0] = (sphere_t){{0, -1, 3}, 1, 0xFFFF0000};  // 红色球体
    spheres[1] = (sphere_t){{2, 0, 4}, 1, 0xFF0000FF};   // 蓝色球体
    spheres[2] = (sphere_t){{-2, 0, 4}, 1, 0xFF00FF00};  // 绿色球体
} 