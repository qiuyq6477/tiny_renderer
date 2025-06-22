#include "raster.h"
#include "display.h"
#include <stdlib.h>
#include <math.h>

// 辅助插值函数
void interpolate(int i0, int d0, int i1, int d1, int* out, int* out_len) {
    int n = abs(i1 - i0) + 1;
    float a = (n > 1) ? (float)(d1 - d0) / (i1 - i0) : 0.0f;
    float d = d0;
    for (int idx = 0, i = i0; idx < n; ++idx, ++i) {
        out[idx] = (int)(d + 0.5f);
        d += a;
    }
    if (out_len) *out_len = n;
}

void draw_line(vec2_t p0, vec2_t p1, uint32_t color)
{
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    if (abs(dx) > abs(dy)) {
        // 横向主线
        if (dx < 0) { vec2_t tmp = p0; p0 = p1; p1 = tmp; dx = -dx; dy = -dy; }
        int len;
        int ys[2048]; // 假设线段不会超过2048像素
        interpolate(p0.x, p0.y, p1.x, p1.y, ys, &len);
        for (int x = p0.x, i = 0; x <= p1.x && i < len; ++x, ++i) {
            draw_pixel(x, ys[i], color);
        }
    } else {
        // 纵向主线
        if (dy < 0) { vec2_t tmp = p0; p0 = p1; p1 = tmp; dx = -dx; dy = -dy; }
        int len;
        int xs[2048];
        interpolate(p0.y, p0.x, p1.y, p1.x, xs, &len);
        for (int y = p0.y, i = 0; y <= p1.y && i < len; ++y, ++i) {
            draw_pixel(xs[i], y, color);
        }
    }
} 

void draw_wireframe_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
    draw_line(p0, p1, color);
    draw_line(p1, p2, color);
    draw_line(p0, p2, color);
}

void draw_filled_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
    // 按y坐标从下到上排序三个点
    if (p1.y < p0.y) { vec2_t tmp = p0; p0 = p1; p1 = tmp; }
    if (p2.y < p0.y) { vec2_t tmp = p0; p0 = p2; p2 = tmp; }
    if (p2.y < p1.y) { vec2_t tmp = p1; p1 = p2; p2 = tmp; }

    // 计算三条边的x坐标
    int x01_len, x12_len, x02_len;
    int x01[2048], x12[2048], x02[2048];
    
    interpolate(p0.y, p0.x, p1.y, p1.x, x01, &x01_len);
    interpolate(p1.y, p1.x, p2.y, p2.x, x12, &x12_len);
    interpolate(p0.y, p0.x, p2.y, p2.x, x02, &x02_len);

    // 合并两条短边
    int x012[4096]; // 足够大的缓冲区
    int x012_len = 0;
    
    // 复制x01（除了最后一个点）
    for (int i = 0; i < x01_len - 1; i++) {
        x012[x012_len++] = x01[i];
    }
    // 复制x12
    for (int i = 0; i < x12_len; i++) {
        x012[x012_len++] = x12[i];
    }

    // 确定哪边是左边，哪边是右边
    int* x_left, *x_right;
    int m = x02_len / 2;
    if (x02[m] < x012[m]) {
        x_left = x02;
        x_right = x012;
    } else {
        x_left = x012;
        x_right = x02;
    }

    // 绘制水平线段
    for (int y = p0.y; y <= p2.y; y++) {
        int idx = y - p0.y;
        if (idx < x02_len && idx < x012_len) {
            for (int x = x_left[idx]; x <= x_right[idx]; x++) {
                draw_pixel(x, y, color);
            }
        }
    }
}
