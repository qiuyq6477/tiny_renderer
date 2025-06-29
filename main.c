#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "raytracer.h"
#include "matrix.h"
#include "raster.h"
#include "geometry.h"

bool is_running = false;

void setup(void) {
    // 分配颜色缓冲区内存
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

    // 创建SDL纹理用于显示颜色缓冲区
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    // 初始化场景
    init_scene();
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    is_running = false;
                break;
        }
    }
}

void update(void) {
    // Raytracing不需要更新逻辑
}

void raytracer_test()
{
    // 对每个像素进行光线追踪
    for (int x = -window_width/2; x < window_width/2; x++) {
        for (int y = -window_height/2; y < window_height/2; y++) {
            vec3_t direction = canvas_to_viewport(x, y);
            direction = normalize(direction);
            direction = matrix_mul_vec3(camera_rotation, direction);

            uint32_t color = trace_ray(camera_position, direction, 1, INFINITY, 3);
            
            // 绘制像素
            draw_pixel(
                x,
                y,
                color
            );
        }
    }
}

void draw_cube()
{
    // 定义立方体的8个顶点
    vec3_t vA = { -2, -0.5, 5 };
    vec3_t vB = { -2,  0.5, 5 };
    vec3_t vC = { -1,  0.5, 5 };
    vec3_t vD = { -1, -0.5, 5 };

    vec3_t vAb = { -2, -0.5, 6 };
    vec3_t vBb = { -2,  0.5, 6 };
    vec3_t vCb = { -1,  0.5, 6 };
    vec3_t vDb = { -1, -0.5, 6 };

    // 定义颜色
    uint32_t RED   = 0xFFFF0000;
    uint32_t GREEN = 0xFF00FF00;
    uint32_t BLUE  = 0xFF0000FF;

    // 投影参数
    float viewport_size = 1.0f;
    float projection_plane_z = 1.0f;

    // 绘制前方正方形
    draw_line(
        project_vertex(vA, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vB, window_width, window_height, viewport_size, projection_plane_z),
        BLUE
    );
    draw_line(
        project_vertex(vB, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vC, window_width, window_height, viewport_size, projection_plane_z),
        BLUE
    );
    draw_line(
        project_vertex(vC, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vD, window_width, window_height, viewport_size, projection_plane_z),
        BLUE
    );
    draw_line(
        project_vertex(vD, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vA, window_width, window_height, viewport_size, projection_plane_z),
        BLUE
    );

    // 绘制后方正方形
    draw_line(
        project_vertex(vAb, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vBb, window_width, window_height, viewport_size, projection_plane_z),
        RED
    );
    draw_line(
        project_vertex(vBb, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vCb, window_width, window_height, viewport_size, projection_plane_z),
        RED
    );
    draw_line(
        project_vertex(vCb, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vDb, window_width, window_height, viewport_size, projection_plane_z),
        RED
    );
    draw_line(
        project_vertex(vDb, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vAb, window_width, window_height, viewport_size, projection_plane_z),
        RED
    );

    // 绘制连接前后方的线
    draw_line(
        project_vertex(vA, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vAb, window_width, window_height, viewport_size, projection_plane_z),
        GREEN
    );
    draw_line(
        project_vertex(vB, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vBb, window_width, window_height, viewport_size, projection_plane_z),
        GREEN
    );
    draw_line(
        project_vertex(vC, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vCb, window_width, window_height, viewport_size, projection_plane_z),
        GREEN
    );
    draw_line(
        project_vertex(vD, window_width, window_height, viewport_size, projection_plane_z),
        project_vertex(vDb, window_width, window_height, viewport_size, projection_plane_z),
        GREEN
    );
}


void clipping_test()
{
    vec3_t cube_vertexes[] = {
        {  1,  1,  1 },
        { -1,  1,  1 },
        { -1, -1,  1 },
        {  1, -1,  1 },
        {  1,  1, -1 },
        { -1,  1, -1 },
        { -1, -1, -1 },
        {  1, -1, -1 }
    };

    triangle_t cube_triangles[] = {
        {0, 1, 2, COLOR_RED},
        {0, 2, 3, COLOR_RED},
        {4, 0, 3, COLOR_GREEN},
        {4, 3, 7, COLOR_GREEN},
        {5, 4, 7, COLOR_BLUE},
        {5, 7, 6, COLOR_BLUE},
        {1, 5, 6, COLOR_YELLOW},
        {1, 6, 2, COLOR_YELLOW},
        {4, 5, 1, COLOR_PURPLE},
        {4, 1, 0, COLOR_PURPLE},
        {2, 6, 7, COLOR_CYAN},
        {2, 7, 3, COLOR_CYAN}
    };

    model_t cube = {
        .vertexes = cube_vertexes,
        .vertex_count = 8,
        .triangles = cube_triangles,
        .triangle_count = 12,
        .bounds_center = {0, 0, 0},
        .bounds_radius = 1.73205f // sqrt(3)
    };

    instance_t instances[] = {
        { 
            .model = &cube, 
            .position = { -1.5f, 0.0f, 7.0f }, 
            .orientation = matrix_identity(4),
            .scale = 0.75
        },
        { 
            .model = &cube, 
            .position = {  1.25f, 2.5f, 7.5f },
            .orientation = matrix_make_oy_rotation(195),
            .scale = 1
        },

    };

    float s2 = 0.70710678f; // sqrt(2)/2
    plane_t clipping_planes[] = {
        { { 0, 0, 1 }, -1 },           // Near
        { { s2, 0, s2 }, 0 },          // Left
        { { -s2, 0, s2 }, 0 },         // Right
        { { 0, -s2, s2 }, 0 },         // Top
        { { 0, s2, s2 }, 0 }           // Bottom
    };
    int clipping_plane_count = 5;

    camera_t camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .orientation = matrix_make_oy_rotation(-30),
        .clipping_planes = clipping_planes,
        .clipping_plane_count = 5,
    };

    render_scene(camera, instances, 2);
}



void raster_test()
{
    // draw_line((vec2_t){-200, -100}, (vec2_t){240, 120}, 0xFFFFFFFF);
    // draw_line((vec2_t){-50, -200}, (vec2_t){60, 240}, 0xFFFFFFFF);

    // vec2_t p0 = (vec2_t){-200, -250};
    // vec2_t p1 = (vec2_t){200, 50};
    // vec2_t p2 = (vec2_t){20, 250};

    // draw_wireframe_triangle(p0, p1, p2, 0xFFFF0000);
    // draw_filled_triangle(p0, p1, p2, 0xFFFF0000);
    // draw_shaded_triangle(p0, 0.3, p1, 0.1, p2, 1, 0xFF00FF00);
    // draw_cube();
    clipping_test();
}

void render(void) {
    // 清空颜色缓冲区
    clear_color_buffer(0xFF000000);
    
    // raytracer_test();
    raster_test();
    // 渲染颜色缓冲区
    render_color_buffer();
    SDL_RenderPresent(renderer);
}

int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
        
        // 添加小延迟以控制帧率
        SDL_Delay(16); // 约60 FPS
    }

    destroy_window();

    return 0;
}
