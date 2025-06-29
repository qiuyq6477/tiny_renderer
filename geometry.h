#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdint.h>
#include "vector.h"
#include "matrix.h"

typedef struct {
    int v0, v1, v2; // 顶点索引
    uint32_t color;
} triangle_t;

// 2D三角形结构体
typedef struct {
    vec2_t v0;
    vec2_t v1;
    vec2_t v2;
    uint32_t color;
} triangle2_t;

// 3D三角形结构体
typedef struct {
    vec3_t v0;
    vec3_t v1;
    vec3_t v2;
    uint32_t color;
} triangle3_t;

// 3D模型结构体
typedef struct {
    vec3_t* vertexes;        // 顶点数组
    int vertex_count;        // 顶点数量
    triangle_t* triangles;  // 三角形数组
    int triangle_count;      // 三角形数量
    vec3_t bounds_center;    // 包围球中心
    float bounds_radius;     // 包围球半径
} model_t;

// 实例结构体
typedef struct {
    model_t* model;          // 指向模型
    vec3_t position;         // 位置
    // 4x4变换矩阵，行优先，长度16
    matrix_t orientation;    // 方向（旋转/变换矩阵）
    float scale;             // 缩放
} instance_t;

// 相机结构体
typedef struct {
    vec3_t position;         // 相机位置
    matrix_t orientation;    // 相机方向（4x4矩阵）
    // 裁剪平面数组
    struct plane_t* clipping_planes;
    int clipping_plane_count;
} camera_t;

// 裁剪平面结构体
typedef struct plane_t {
    vec3_t normal;           // 法向量
    float distance;          // 距离
} plane_t;



#endif // GEOMETRY_H 