#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "DynamicArray/array.h"
#include "matrix.h"
#include "light.h"

triangle_t *triangles_to_render = NULL;

bool is_running = false;
uint32_t previous_frame_time = 0;

vec3_t camera_position = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
mat4_t projection_matrix;

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

    float fov = M_PI / 3.0f;
    float aspect = (float)window_height / (float)window_width;
    float z_near = 0.1f;
    float z_far = 100.0f;

    projection_matrix = mat4_make_perspective(fov, aspect, z_near, z_far);

    load_obj_file_data("../assets/f22.obj");
    // load_cube_mesh_data();
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

    mesh.rotation.x += 0.03f;
    // mesh.rotation.y += 0.03f;
    // mesh.rotation.z += 0.03f;

    // mesh.scale.x += 0.0001f;
    // mesh.scale.y += 0.0001f;

    // mesh.translation.x += 0.001f;
    mesh.translation.z = 5.0f;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

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

            mat4_t world_matrix = mat4_identity();

            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

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

        if (cull_method == CULL_BACKFACE)
        {
            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        vec4_t projected_points[3];

        for (int j = 0; j < 3; j++)
        {
            projected_points[j] = mat4_mul_vec4_projection(projection_matrix, transformed_vertices[j]);

            projected_points[j].x *= (window_width / 2.0f);
            projected_points[j].y *= (window_height / 2.0f);

            projected_points[j].x += (window_width / 2.0f);
            projected_points[j].y += (window_height / 2.0f);
        }

        float depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0f;

        float light_intensity_factor = -vec3_dot(normal, light.direction);

        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle =
        {
            .points =
                {
                    {projected_points[0].x, projected_points[0].y},
                    {projected_points[1].x, projected_points[1].y},
                    {projected_points[2].x, projected_points[2].y}
                },
            .color = triangle_color,
            .avg_depth = depth
        };

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