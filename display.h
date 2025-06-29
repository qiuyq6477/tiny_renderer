#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define COLOR_RED    0xFFFF0000
#define COLOR_GREEN  0xFF00FF00
#define COLOR_BLUE   0xFF0000FF
#define COLOR_YELLOW 0xFFFFFF00
#define COLOR_PURPLE 0xFFFF00FF
#define COLOR_CYAN   0xFF00FFFF

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;

bool initialize_window(void); 
void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer(void); 
void clear_color_buffer(uint32_t color);
void destroy_window(void);

#endif
