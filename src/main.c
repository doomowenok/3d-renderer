#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"

const int N_POINTS = 9 * 9 * 9;
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = {.x = 0.0f, .y = 0.0f, .z = -5.0f};
vec3_t cube_rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

float fov_factor = 640;

bool is_running = false;
uint32_t previous_frame_time = 0;

void setup(void)
{
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    int point_count = 0;

    for (float x = -1.0f; x < 1.0f; x += 0.25f)
    {
        for (float y = -1.0f; y < 1.0f; y += 0.25f)
        {
            for (float z = -1.0f; z < 1.0f; z += 0.25f)
            {
                const vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[point_count] = new_point;
                point_count++;
            }
        }
    }
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
            }
            break;
        default:
            break;
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point =
    {
        .x = (point.x * fov_factor) / point.z,
        .y = (point.y * fov_factor) / point.z,
    };

    return projected_point;
}

void update(void)
{
    const int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    cube_rotation.y += 0.01f;
    cube_rotation.z += 0.01f;
    cube_rotation.x += 0.01f;

    for (int i = 0; i < N_POINTS; i++)
    {
        vec3_t point = cube_points[i];

        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);
        
        transformed_point.z -= camera_position.z;

        vec2_t projected_point = project(transformed_point);

        projected_points[i] = projected_point;
    }
}

void render(void)
{
    // draw_grid(10, 0xFFFFFFFF);

    for (int i = 0; i < N_POINTS; i++)
    {
        vec2_t projected_point = projected_points[i];
        draw_rect(
            projected_point.x + window_width / 2.0f,
            projected_point.y + window_height / 2.0f,
            4,
            4,
            0xFFFFFF00
        );
    }

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

int main(void)
{
    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
