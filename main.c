#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "raytracer.h"
#include "matrix.h"
#include "raster.h"

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
            direction = matrix_mul_vec3(&camera_rotation, direction);

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

void raster_test()
{
    // draw_line((vec2_t){-200, -100}, (vec2_t){240, 120}, 0xFFFFFFFF);
    // draw_line((vec2_t){-50, -200}, (vec2_t){60, 240}, 0xFFFFFFFF);

    vec2_t p0 = (vec2_t){-200, -250};
    vec2_t p1 = (vec2_t){200, 50};
    vec2_t p2 = (vec2_t){20, 250};

    draw_wireframe_triangle(p0, p1, p2, 0xFF000000);
    // draw_filled_triangle(p0, p1, p2, 0xFFFF0000);
    draw_shaded_triangle(p0, 0.3, p1, 0.1, p2, 1, 0xFF00FF00);
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
