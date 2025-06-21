#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "raytracer.h"

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

void render(void) {
    // 清空颜色缓冲区
    clear_color_buffer(0xFF000000);
    
    // 对每个像素进行光线追踪
    for (int x = -window_width/2; x < window_width/2; x++) {
        for (int y = -window_height/2; y < window_height/2; y++) {
            vec3_t direction = canvas_to_viewport(x, y);
            direction = normalize(direction);
            
            uint32_t color = trace_ray(camera_position, direction, 1, INFINITY, 3);
            
            // 绘制像素
            draw_pixel(
                window_width/2 + x,
                window_height/2 - y,
                color
            );
        }
    }
    
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
