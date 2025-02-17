#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define FPS 144
#define FRAME_TARGET_TIME (1000 / FPS)

enum cull_method
{
    CULL_NONE,
    CULL_BACKFACE
};

enum render_method
{
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
};

bool initialize_window(void);

int get_window_width(void);
int get_window_height(void);

void set_render_method(int method);
void set_cull_method(int method);

bool is_cull_backface(void);
bool should_render_filled_triangles(void);
bool should_render_textured_triangles(void);
bool should_render_wireframe(void);
bool should_render_wire_vertex(void);

void draw_grid(const int per_segment, const uint32_t color);
void draw_pixel(const int x, const int y, const uint32_t color);
void draw_line(const int x0, const int y0, const int x1, const int y1, const uint32_t color);
void draw_rect(const int x, const int y, const int width, const  int height, const uint32_t color);

void render_color_buffer(void);
void clear_color_buffer(const uint32_t color);
void clear_z_buffer();

float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float value);

void destroy_window(void);

#endif