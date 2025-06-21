#include "raytracer.h"
#include "display.h"
#include <math.h>
#include <stddef.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

// 全局变量定义
sphere_t spheres[NUM_SPHERES];
light_t lights[NUM_LIGHTS];
vec3_t camera_position = {0, 0, 0};

// 向量点积
float dot_product(vec3_t v1, vec3_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 向量长度
float length(vec3_t v) {
    return sqrt(dot_product(v, v));
}

// 向量乘法
vec3_t multiply(vec3_t v, float k) {
    vec3_t result = {v.x * k, v.y * k, v.z * k};
    return result;
}

// 向量加法
vec3_t add(vec3_t v1, vec3_t v2) {
    vec3_t result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    return result;
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
    
    vec3_t co = subtract(origin, sphere.center);
    
    float k1 = dot_product(direction, direction);
    float k2 = 2 * dot_product(co, direction);
    float k3 = dot_product(co, co) - sphere.radius * sphere.radius;
    
    float discriminant = k2 * k2 - 4 * k1 * k3;
    if (discriminant < 0) {
        return result;
    }
    
    result.t1 = (-k2 + sqrt(discriminant)) / (2 * k1);
    result.t2 = (-k2 - sqrt(discriminant)) / (2 * k1);
    
    return result;
}

// 计算光照
float compute_lighting(vec3_t point, vec3_t normal, vec3_t view, float specular) {
    float intensity = 0.0f;
    float length_n = length(normal);
    float length_v = length(view);

    for (int i = 0; i < NUM_LIGHTS; i++) {
        light_t light = lights[i];
        if (light.ltype == LIGHT_AMBIENT) {
            intensity += light.intensity;
        } else {
            vec3_t vec_l;
            if (light.ltype == LIGHT_POINT) {
                vec_l = subtract(light.position, point);
            } else {
                vec_l = light.position;
            }

            // 漫反射
            float n_dot_l = dot_product(normal, vec_l);
            if (n_dot_l > 0) {
                intensity += light.intensity * n_dot_l / (length_n * length(vec_l));
            }

            // 镜面反射
            if (specular != -1) {
                vec3_t vec_r = subtract(multiply(normal, 2 * dot_product(normal, vec_l)), vec_l);
                float r_dot_v = dot_product(vec_r, view);
                if (r_dot_v > 0) {
                    intensity += light.intensity * pow(r_dot_v / (length(vec_r) * length_v), specular);
                }
            }   
        }
    }
    return intensity;
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
    
    vec3_t point = add(origin, multiply(direction, closest_t));
    vec3_t normal = normalize(subtract(point, closest_sphere->center));

    vec3_t view = multiply(direction, -1);
    float lighting = compute_lighting(point, normal, view, closest_sphere->specular);
    uint32_t color = closest_sphere->color;
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = min(255, (int)(r * lighting));
    g = min(255, (int)(g * lighting));
    b = min(255, (int)(b * lighting));

    return (a << 24) | (r << 16) | (g << 8) | b;
}

// 初始化场景
void init_scene(void) {
    // 初始化球体
    spheres[0] = (sphere_t){{0, -1, 3}, 1, 0xFFFF0000, 500};  // 红色球体
    spheres[1] = (sphere_t){{2, 0, 4}, 1, 0xFF0000FF, 500};   // 蓝色球体
    spheres[2] = (sphere_t){{-2, 0, 4}, 1, 0xFF00FF00, 10};  // 绿色球体
    spheres[3] = (sphere_t){{0, -5001, 0}, 5000, 0xFFFFFF00, 1000};  // 黄色球体

    // 初始化光源
    lights[0] = (light_t){LIGHT_AMBIENT, 0.2, {0, 0, 0}};
    lights[1] = (light_t){LIGHT_POINT, 0.6, {2, 1, 0}};
    lights[2] = (light_t){LIGHT_DIRECTIONAL, 0.2, {1, 4, 4}};
} 