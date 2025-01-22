#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "DynamicArray/array.h"

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

float fov_factor = 640;

bool is_running = false;
uint32_t previous_frame_time = 0;

void setup(void)
{
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);

    load_obj_file_data("../assets/cube.obj");
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

    triangles_to_render = NULL;

    mesh.rotation.y += 0.05f;
    mesh.rotation.z += 0.05f;
    mesh.rotation.x += 0.05f;

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z += 5.0f;

            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t a = transformed_vertices[0];
        vec3_t b = transformed_vertices[1];
        vec3_t c = transformed_vertices[2];

        vec3_t ab = vec3_sub(b, a);
        vec3_t ac = vec3_sub(c, a);
        vec3_t camera_ray = vec3_sub(camera_position, a);

        vec3_t normal = vec3_cross(ab, ac);
        float dot = vec3_dot(normal, camera_ray);

        if (dot < 0) continue;

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++)
        {
            vec2_t projected_point = project(transformed_vertices[j]);

            projected_point.x += window_width / 2.0f;
            projected_point.y += window_height / 2.0f;

            projected_triangle.points[j] = projected_point;
        }

        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void)
{
    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle_to_render = triangles_to_render[i];

        draw_rect(triangle_to_render.points[0].x, triangle_to_render.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle_to_render.points[1].x, triangle_to_render.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle_to_render.points[2].x, triangle_to_render.points[2].y, 3, 3, 0xFFFFFF00);

        draw_triangle(
            triangle_to_render.points[0].x,
            triangle_to_render.points[0].y,
            triangle_to_render.points[1].x,
            triangle_to_render.points[1].y,
            triangle_to_render.points[2].x,
            triangle_to_render.points[2].y,
            0xFFFFFF00);
    }

    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void free_resources()
{
    array_free(mesh.faces);
    array_free(mesh.vertices);
    array_free(color_buffer);
    array_free(triangles_to_render);
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
    free_resources();

    return 0;
}
