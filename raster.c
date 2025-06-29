#include <stdlib.h>
#include <math.h>
#include "raster.h"
#include "display.h"
#include "geometry.h"
#include "matrix.h"

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

// 裁剪三角形，输出到triangles_out，返回新三角形数量
static int clip_triangle(
    triangle_t tri,
    plane_t plane,
    vec3_t* vertexes,
    triangle_t* triangles_out,
    vec3_t* new_vertexes,
    int* new_vertex_count,
    int max_vertex
) {
    int idx[3] = {tri.v0, tri.v1, tri.v2};
    float d[3];
    int in_idx[3], out_idx[3], in_count = 0, out_count = 0;
    for (int i = 0; i < 3; i++) {
        d[i] = vec3_dot(plane.normal, vertexes[idx[i]]) + plane.distance;
        if (d[i] > 0) in_idx[in_count++] = i;
        else out_idx[out_count++] = i;
    }
    if (in_count == 0) return 0;
    if (in_count == 3) {
        triangles_out[0] = tri;
        return 1;
    }
    if (in_count == 1 && out_count == 2) {
        int i0 = in_idx[0], i1 = out_idx[0], i2 = out_idx[1];
        int v0 = idx[i0], v1 = idx[i1], v2 = idx[i2];
        float t1 = d[i0] / (d[i0] - d[i1]);
        float t2 = d[i0] / (d[i0] - d[i2]);
        if (*new_vertex_count + 2 >= max_vertex) return 0; // 防止越界
        vec3_t p1 = vec3_add(vertexes[v0], vec3_scale(vec3_sub(vertexes[v1], vertexes[v0]), t1));
        vec3_t p2 = vec3_add(vertexes[v0], vec3_scale(vec3_sub(vertexes[v2], vertexes[v0]), t2));
        int p1_idx = (*new_vertex_count)++;
        int p2_idx = (*new_vertex_count)++;
        new_vertexes[p1_idx] = p1;
        new_vertexes[p2_idx] = p2;
        triangles_out[0] = (triangle_t){v0, p1_idx, p2_idx, tri.color};
        return 1;
    }
    if (in_count == 2 && out_count == 1) {
        int i0 = in_idx[0], i1 = in_idx[1], i2 = out_idx[0];
        int v0 = idx[i0], v1 = idx[i1], v2 = idx[i2];
        float t0 = d[i0] / (d[i0] - d[i2]);
        float t1 = d[i1] / (d[i1] - d[i2]);
        if (*new_vertex_count + 2 >= max_vertex) return 0; // 防止越界
        vec3_t p0 = vec3_add(vertexes[v0], vec3_scale(vec3_sub(vertexes[v2], vertexes[v0]), t0));
        vec3_t p1 = vec3_add(vertexes[v1], vec3_scale(vec3_sub(vertexes[v2], vertexes[v1]), t1));
        int p0_idx = (*new_vertex_count)++;
        int p1_idx = (*new_vertex_count)++;
        new_vertexes[p0_idx] = p0;
        new_vertexes[p1_idx] = p1;
        triangles_out[0] = (triangle_t){v0, v1, p0_idx, tri.color};
        triangles_out[1] = (triangle_t){v1, p1_idx, p0_idx, tri.color};
        return 2;
    }
    return 0;
}

// 变换和裁剪模型
model_t* transform_and_clip(
    plane_t* planes, int plane_count,
    model_t* model, matrix_t* transform
) {
    // 1. 变换所有顶点
    int max_vertex = model->vertex_count + model->triangle_count * 256; // 放大，防止越界
    vec3_t* vertexes = malloc(sizeof(vec3_t) * max_vertex);
    for (int i = 0; i < model->vertex_count; i++) {
        // 只取前三维
        vec4_t v4 = {model->vertexes[i].x, model->vertexes[i].y, model->vertexes[i].z, 1.0f};
        vec4_t tv = matrix_mul_vec4(*transform, v4);
        vertexes[i] = (vec3_t){tv.x, tv.y, tv.z};
    }
    int vertex_count = model->vertex_count;

    // 2. 裁剪三角形
    int max_tri = model->triangle_count * 32; // 放大
    triangle_t* triangles = malloc(sizeof(triangle_t) * max_tri);
    memcpy(triangles, model->triangles, sizeof(triangle_t) * model->triangle_count);
    int triangle_count = model->triangle_count;

    for (int p = 0; p < plane_count; p++) {
        triangle_t* new_tris = malloc(sizeof(triangle_t) * max_tri);
        int new_count = 0;
        for (int t = 0; t < triangle_count; t++) {
            new_count += clip_triangle(triangles[t], planes[p], vertexes, &new_tris[new_count], vertexes, &vertex_count, max_vertex);
        }
        free(triangles);
        triangles = new_tris;
        triangle_count = new_count;
    }

    model_t* result = malloc(sizeof(model_t));
    result->vertexes = vertexes;
    result->vertex_count = vertex_count;
    result->triangles = triangles;
    result->triangle_count = triangle_count;
    result->bounds_center = model->bounds_center;
    result->bounds_radius = model->bounds_radius;
    return result;
}

// 投影并绘制三角形
void render_wireframe_model(const model_t* model) {
    float viewport_size = 1.0f;
    float projection_plane_z = 1.0f;
    for (int i = 0; i < model->triangle_count; i++) {
        triangle_t tri = model->triangles[i];
        vec3_t v0 = model->vertexes[tri.v0];
        vec3_t v1 = model->vertexes[tri.v1];
        vec3_t v2 = model->vertexes[tri.v2];
        vec2_t p0 = project_vertex(v0, window_width, window_height, viewport_size, projection_plane_z);
        vec2_t p1 = project_vertex(v1, window_width, window_height, viewport_size, projection_plane_z);
        vec2_t p2 = project_vertex(v2, window_width, window_height, viewport_size, projection_plane_z);
        draw_wireframe_triangle(p0, p1, p2, tri.color);
    }
}

#pragma region 深度测试和背面剔除

// 计算三角形法线
static vec3_t compute_triangle_normal(vec3_t v0, vec3_t v1, vec3_t v2) {
    vec3_t v0v1 = vec3_sub(v1, v0);
    vec3_t v0v2 = vec3_sub(v2, v0);
    return vec3_cross(v0v1, v0v2);
}


// 判断三角形是否为背面（视点在原点）
static bool is_backface(vec3_t v0, vec3_t v1, vec3_t v2) {
    vec3_t normal = compute_triangle_normal(v0, v1, v2);
    // 取三角形中心到原点的向量
    vec3_t center = vec3_scale(vec3_add(vec3_add(v0, v1), v2), -1.0f/3.0f);
    // 视点在原点，normal和center点积<0为背面
    return vec3_dot(center, normal) < 0;
}

// 深度缓冲区
static float* depth_buffer = NULL;
static int depth_buffer_w = 0, depth_buffer_h = 0;

void clear_depth_buffer(int w, int h) {
    if (!depth_buffer || depth_buffer_w != w || depth_buffer_h != h) {
        if (depth_buffer) free(depth_buffer);
        depth_buffer = malloc(sizeof(float) * w * h);
        depth_buffer_w = w;
        depth_buffer_h = h;
    }
    for (int i = 0; i < w * h; ++i) depth_buffer[i] = -INFINITY;
}

static bool update_depth_buffer_if_closer(int x, int y, float inv_z) {
    x = window_width/2 + x;
    y = window_height/2 - y;
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) return false;
    int offset = x + window_width * y;
    if (depth_buffer[offset] < inv_z) {
        depth_buffer[offset] = inv_z;
        return true;
    }
    return false;
}

// 控制开关
static bool g_enable_depth_test = true;
static bool g_enable_backface_cull = true;
static bool g_enable_triangle_outline = false;

void set_depth_test_enabled(bool enabled) { g_enable_depth_test = enabled; }
void set_backface_cull_enabled(bool enabled) { g_enable_backface_cull = enabled; }
void set_triangle_outline_enabled(bool enabled) { g_enable_triangle_outline = enabled; }

// 填充三角形（带深度测试和背面剔除，可开关，支持描边）
void render_filled_model(const model_t* model) {
    float viewport_size = 1.0f;
    float projection_plane_z = 1.0f;
    if (g_enable_depth_test) {
        clear_depth_buffer(window_width, window_height);
    }
    for (int i = 0; i < model->triangle_count; i++) {
        triangle_t tri = model->triangles[i];
        vec3_t v0 = model->vertexes[tri.v0];
        vec3_t v1 = model->vertexes[tri.v1];
        vec3_t v2 = model->vertexes[tri.v2];
        if (g_enable_backface_cull && is_backface(v0, v1, v2)) continue;
        // 投影
        vec2_t p0 = project_vertex(v0, window_width, window_height, viewport_size, projection_plane_z);
        vec2_t p1 = project_vertex(v1, window_width, window_height, viewport_size, projection_plane_z);
        vec2_t p2 = project_vertex(v2, window_width, window_height, viewport_size, projection_plane_z);
        // 按y排序
        if (p1.y < p0.y) { vec2_t tp = p0; p0 = p1; p1 = tp; float tz = v0.z; v0.z = v1.z; v1.z = tz; }
        if (p2.y < p0.y) { vec2_t tp = p0; p0 = p2; p2 = tp; float tz = v0.z; v0.z = v2.z; v2.z = tz; }
        if (p2.y < p1.y) { vec2_t tp = p1; p1 = p2; p2 = tp; float tz = v1.z; v1.z = v2.z; v2.z = tz; }
        // 计算三条边的x坐标和1/z插值
        int x01_len, x12_len, x02_len;
        int x01[2048], x12[2048], x02[2048];
        interpolate(p0.y, p0.x, p1.y, p1.x, x01, &x01_len);
        interpolate(p1.y, p1.x, p2.y, p2.x, x12, &x12_len);
        interpolate(p0.y, p0.x, p2.y, p2.x, x02, &x02_len);
        // 1/z插值
        float iz0 = 1.0f/v0.z, iz1 = 1.0f/v1.z, iz2 = 1.0f/v2.z;
        int iz01_len, iz12_len, iz02_len;
        float iz01[2048], iz12[2048], iz02[2048];
        interpolate_float(p0.y, iz0, p1.y, iz1, iz01, &iz01_len);
        interpolate_float(p1.y, iz1, p2.y, iz2, iz12, &iz12_len);
        interpolate_float(p0.y, iz0, p2.y, iz2, iz02, &iz02_len);
        // 合并两条短边
        int x012[4096], x012_len = 0;
        float iz012[4096]; int iz012_len = 0;
        for (int i = 0; i < x01_len - 1; i++) { x012[x012_len++] = x01[i]; iz012[iz012_len++] = iz01[i]; }
        for (int i = 0; i < x12_len; i++) { x012[x012_len++] = x12[i]; iz012[iz012_len++] = iz12[i]; }
        // 确定哪边是左边，哪边是右边
        int* x_left; int* x_right; float* iz_left; float* iz_right;
        int m = x02_len / 2;
        if (x02[m] < x012[m]) { x_left = x02; iz_left = iz02; x_right = x012; iz_right = iz012; }
        else { x_left = x012; iz_left = iz012; x_right = x02; iz_right = iz02; }
        // 绘制水平线段
        for (int y = p0.y; y <= p2.y; y++) {
            int idx = y - p0.y;
            if (idx < x02_len && idx < x012_len) {
                int xl = x_left[idx];
                int xr = x_right[idx];
                float izl = iz_left[idx];
                float izr = iz_right[idx];
                int seg_len = xr - xl + 1;
                if (seg_len <= 0) continue;
                float izstep = (izr - izl) / (float)(xr - xl == 0 ? 1 : xr - xl);
                float izval = izl;
                for (int x = xl; x <= xr; x++) {
                    bool pass_depth = true;
                    if (g_enable_depth_test) {
                        pass_depth = update_depth_buffer_if_closer(x, y, izval);
                    }
                    if (pass_depth) {
                        draw_pixel(x, y, tri.color);
                    }
                    izval += izstep;
                }
            }
        }
        // 三角形描边
        if (g_enable_triangle_outline) {
            // 取较暗色
            uint32_t outline_color = (tri.color & 0xFF000000) | (((((tri.color>>16)&0xFF)*3/4)<<16) | ((((tri.color>>8)&0xFF)*3/4)<<8) | (((tri.color)&0xFF)*3/4));
            draw_wireframe_triangle(p0, p1, p2, outline_color);
        }
    }
}

#pragma endregion

void render_scene(const camera_t camera, const instance_t *instances, int instance_count) {
    // 1. 计算相机变换矩阵
    matrix_t camera_matrix = matrix_mul(
        matrix_transpose(camera.orientation),
        matrix_make_translation(vec3_neg(camera.position))
    );

    for (int i = 0; i < instance_count; i++) {
        // 2. 计算模型变换矩阵
        matrix_t model_matrix = matrix_mul(
            matrix_make_translation(instances[i].position),
            matrix_mul(instances[i].orientation, matrix_make_scaling(instances[i].scale))
        );
        matrix_t transform = matrix_mul(camera_matrix, model_matrix);

        // 3. 变换并裁剪
        model_t* clipped = transform_and_clip(
            camera.clipping_planes, camera.clipping_plane_count,
            instances[i].model, &transform
        );
        if (clipped) {
            // 4. 投影并光栅化
            render_filled_model(clipped);
            // 释放clipped分配的内存
            free(clipped->vertexes);
            free(clipped->triangles);
            free(clipped);
        }
    }
}