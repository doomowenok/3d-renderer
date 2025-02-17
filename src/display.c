#include "display.h"
#include "DynamicArray/array.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t *color_buffer = NULL;
static float *z_buffer = NULL;

static SDL_Texture *color_buffer_texture = NULL;
static int window_width = 800;
static int window_height = 600;

static int render_method;
static int cull_method;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w / 3;
    window_height = display_mode.h / 3;

    window = SDL_CreateWindow(
        "3D Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        0);

    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);

    return true;
}

int get_window_width(void)
{
    return window_width;
}

int get_window_height(void)
{
    return window_height;
}

void set_render_method(int method)
{
    render_method = method;
}

void set_cull_method(int method)
{
    cull_method = method;
}

bool is_cull_backface(void)
{
    return cull_method == CULL_BACKFACE;
}

bool should_render_filled_triangles(void)
{
    return render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED;
}

bool should_render_textured_triangles(void)
{
    return render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE;
}

bool should_render_wireframe(void)
{
    return render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE;
}

bool should_render_wire_vertex(void)
{
    return render_method == RENDER_WIRE_VERTEX;
}

void draw_grid(const int per_segment, const uint32_t color)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            if ((y % per_segment == 0 && y != 0) && (x % per_segment == 0 && x != 0))
            {
                draw_pixel(x, y, color);
            }
        }
    }
}

void draw_pixel(const int x, const int y, const uint32_t color)
{
    if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    {
        return;
    }

    color_buffer[(window_width * y) + x] = color;
}

void draw_line(const int x0, const int y0, const int x1, const int y1, const uint32_t color)
{
    const int delta_x = x1 - x0;
    const int delta_y = y1 - y0;

    const int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    const float x_inc = (float)delta_x / (float)longest_side_length;
    const float y_inc = (float)delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++)
    {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rect(const int x, const int y, const int width, const int height, const uint32_t color)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            const int current_x = x + i;
            const int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void render_color_buffer(void)
{
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t)));

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

void clear_color_buffer(const uint32_t color)
{
    for(int i = 0; i < window_width * window_height; i++)
    {
        color_buffer[i] = color;
    }
}

void clear_z_buffer()
{
    for(int i = 0; i < window_width * window_height; i++)
    {
        z_buffer[i] = 1.0f;
    }
}

float get_zbuffer_at(int x, int y)
{
    if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    {
        return 1.0f;
    }

    return z_buffer[(window_width * y) + x];
}

void update_zbuffer_at(int x, int y, float value)
{
    if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    {
        return;
    }
    
    z_buffer[(window_width * y) + x] = value;
}

void destroy_window(void)
{
    array_free(color_buffer);
    array_free(z_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}