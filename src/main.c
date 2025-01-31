#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "DynamicArray/array.h"
#include "matrix.h"

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

float fov_factor = 640;

bool is_running = false;
uint32_t previous_frame_time = 0;

void setup(void)
{
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);

    // load_obj_file_data("../assets/cube.obj");
    load_cube_mesh_data();
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
        if (event.key.keysym.sym == SDLK_1)
        {
            render_method = RENDER_WIRE_VERTEX;
        }
        if (event.key.keysym.sym == SDLK_2)
        {
            render_method = RENDER_WIRE;
        }
        if (event.key.keysym.sym == SDLK_3)
        {
            render_method = RENDER_FILL_TRIANGLE;
        }
        if (event.key.keysym.sym == SDLK_2)
        {
            render_method = RENDER_FILL_TRIANGLE_WIRE;
        }
        if (event.key.keysym.sym == SDLK_c)
        {
            cull_method = CULL_BACKFACE;
        }
        if (event.key.keysym.sym == SDLK_d)
        {
            cull_method = CULL_NONE;
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

void sort_triangles_by_depth()
{
    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++)
    {
        for (int j = 0; j < num_triangles - i - 1; j++)
        {
            if (triangles_to_render[j].avg_depth < triangles_to_render[j + 1].avg_depth)
            {
                triangle_t temp = triangles_to_render[j];
                triangles_to_render[j] = triangles_to_render[j + 1];
                triangles_to_render[j + 1] = temp;
            }
        }
    }
}

void update(void)
{
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    mesh.scale.x += 0.01f;
    mesh.scale.y += 0.02f;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);

            // transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            // transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            // transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z += 5;

            transformed_vertices[j] = transformed_vertex;
        }

        if (cull_method == CULL_BACKFACE)
        {
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);

            vec3_t normal = vec3_cross(vector_ab, vector_ac);
            vec3_normalize(&normal);

            vec3_t camera_ray = vec3_sub(camera_position, vector_a);

            float dot_normal_camera = vec3_dot(normal, camera_ray);

            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        float depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0f;

        vec2_t projected_points[3];

        for (int j = 0; j < 3; j++)
        {
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }

        triangle_t projected_triangle =
            {
                .points =
                    {
                        {projected_points[0].x, projected_points[0].y},
                        {projected_points[1].x, projected_points[1].y},
                        {projected_points[2].x, projected_points[2].y}
                    },
                .color = mesh_face.color,
                .avg_depth = depth};

        array_push(triangles_to_render, projected_triangle);
    }

    sort_triangles_by_depth();
}

void render(void)
{
    SDL_RenderClear(renderer);

    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color);
        }

        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color);
        }

        if (render_method == RENDER_WIRE_VERTEX)
        {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }
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