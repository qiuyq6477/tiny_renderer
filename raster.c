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

// 实现 interpolate_float 方法
void interpolate_float(int i0, float d0, int i1, float d1, float* out, int* out_len) {
    if (i0 == i1) {
        out[0] = d0;
        *out_len = 1;
        return;
    }
    int n = abs(i1 - i0) + 1;
    float a = (n > 1) ? (d1 - d0) / (i1 - i0) : 0.0f;
    float d = d0;
    for (int idx = 0, i = i0; idx < n; ++idx, ++i) {
        out[idx] = d;
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

// 实现带有插值的着色三角形绘制函数
// h0, h1, h2 分别为每个顶点的亮度（0.0~1.0），color为基础颜色（ARGB格式）
void draw_shaded_triangle(
    vec2_t p0, float h0,
    vec2_t p1, float h1,
    vec2_t p2, float h2,
    uint32_t color
) {
    // 按y坐标从下到上排序三个点，同时亮度也要跟着调整
    if (p1.y < p0.y) { vec2_t tp = p0; p0 = p1; p1 = tp; float th = h0; h0 = h1; h1 = th; }
    if (p2.y < p0.y) { vec2_t tp = p0; p0 = p2; p2 = tp; float th = h0; h0 = h2; h2 = th; }
    if (p2.y < p1.y) { vec2_t tp = p1; p1 = p2; p2 = tp; float th = h1; h1 = h2; h2 = th; }

    // 计算三条边的x坐标和亮度插值
    int x01_len, x12_len, x02_len;
    int x01[2048], x12[2048], x02[2048];
    int y0 = (int)p0.y, y1 = (int)p1.y, y2 = (int)p2.y;
    int x0 = (int)p0.x, x1 = (int)p1.x, x2 = (int)p2.x;

    interpolate(y0, x0, y1, x1, x01, &x01_len);
    interpolate(y1, x1, y2, x2, x12, &x12_len);
    interpolate(y0, x0, y2, x2, x02, &x02_len);

    // 亮度插值
    int h01_len, h12_len, h02_len;
    float h01[2048], h12[2048], h02[2048];

    interpolate_float(y0, h0, y1, h1, h01, &h01_len);
    interpolate_float(y1, h1, y2, h2, h12, &h12_len);
    interpolate_float(y0, h0, y2, h2, h02, &h02_len);

    // 合并两条短边
    int x012[4096], x012_len = 0;
    float h012[4096];
    int h012_len = 0;
    for (int i = 0; i < x01_len - 1; i++) {
        x012[x012_len++] = x01[i];
        h012[h012_len++] = h01[i];
    }
    for (int i = 0; i < x12_len; i++) {
        x012[x012_len++] = x12[i];
        h012[h012_len++] = h12[i];
    }

    // 确定哪边是左边，哪边是右边
    int* x_left; int* x_right;
    float* h_left; float* h_right;
    int m = x02_len / 2;
    if (x02[m] < x012[m]) {
        x_left = x02; h_left = h02;
        x_right = x012; h_right = h012;
    } else {
        x_left = x012; h_left = h012;
        x_right = x02; h_right = h02;
    }

    // 绘制水平线段
    for (int y = y0; y <= y2; y++) {
        int idx = y - y0;
        if (idx < x02_len && idx < x012_len) {
            int xl = x_left[idx];
            int xr = x_right[idx];
            float hl = h_left[idx];
            float hr = h_right[idx];
            int seg_len = xr - xl + 1;
            if (seg_len <= 0) continue;
            float hstep = (hr - hl) / (float)(xr - xl == 0 ? 1 : xr - xl);
            float hval = hl;
            for (int x = xl; x <= xr; x++) {
                // 计算插值后的颜色
                float shade = hval;
                if (shade < 0.0f) shade = 0.0f;
                if (shade > 1.0f) shade = 1.0f;
                uint8_t a = (color >> 24) & 0xFF;
                uint8_t r = (color >> 16) & 0xFF;
                uint8_t g = (color >> 8) & 0xFF;
                uint8_t b = color & 0xFF;
                uint8_t rr = (uint8_t)(r * shade);
                uint8_t gg = (uint8_t)(g * shade);
                uint8_t bb = (uint8_t)(b * shade);
                uint32_t shaded_color = (a << 24) | (rr << 16) | (gg << 8) | bb;
                draw_pixel(x, y, shaded_color);
                hval += hstep;
            }
        }
    }
}

// 实现视口到画布的坐标变换
vec2_t view_port_to_canvas(vec2_t p, int canvas_width, int canvas_height, float viewport_size) {
    vec2_t result;
    result.x = (int)(p.x * canvas_width / viewport_size);
    result.y = (int)(p.y * canvas_height / viewport_size);
    return result;
}

// 实现三维顶点投影到二维画布
vec2_t project_vertex(vec3_t v, int canvas_width, int canvas_height, float viewport_size, float projection_plane_z) {
    vec2_t projected;
    projected.x = v.x * projection_plane_z / v.z;
    projected.y = v.y * projection_plane_z / v.z;
    return view_port_to_canvas(projected, canvas_width, canvas_height, viewport_size);
}
