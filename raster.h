#ifndef RASTER_H
#define RASTER_H

#include "vector.h"
#include <stdint.h>

// 画线函数，color为ARGB格式
void draw_line(vec2_t p0, vec2_t p1, uint32_t color);

// 优化后的interpolate函数声明
void interpolate(int i0, int d0, int i1, int d1, int* out, int* out_len);

#endif // RASTER_H 