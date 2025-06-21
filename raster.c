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