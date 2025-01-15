#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;

bool initialize_window(void);
void draw_grid(const int per_segment, const uint32_t color);
void draw_pixel(const int x, const int y, const uint32_t color);
void draw_rect(const int x, const int y, const int width, const  int height, const uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(const uint32_t color);
void destroy_window(void);

#endif