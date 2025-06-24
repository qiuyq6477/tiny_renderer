#ifndef RASTER_H
#define RASTER_H

#include "vector.h"
#include <stdint.h>

// 画线函数，color为ARGB格式
void draw_line(vec2_t p0, vec2_t p1, uint32_t color);

// 绘制线框三角形，color为ARGB格式
void draw_wireframe_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

// 绘制填充三角形，color为ARGB格式
void draw_filled_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

// 绘制带有亮度插值的着色三角形，h0/h1/h2为每个顶点的亮度（0.0~1.0），color为基础颜色（ARGB格式）
void draw_shaded_triangle(
    vec2_t p0, float h0,
    vec2_t p1, float h1,
    vec2_t p2, float h2,
    uint32_t color
);

// 优化后的interpolate函数声明
void interpolate(int i0, int d0, int i1, int d1, int* out, int* out_len);

#endif // RASTER_H 