#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "DynamicArray/array.h"
#include "UPNG/upng.h"
#include "matrix.h"
#include "texture.h"
#include "triangle.h"
#include "light.h"
#include "camera.h"
#include "clipping.h"

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
uint num_triangles_to_render = 0;

bool is_running = false;
uint32_t previous_frame_time = 0;
float delta_time = 0.0f;

mat4_t projection_matrix;
mat4_t world_matrix;
mat4_t view_matrix;

void setup(void)
{
    set_render_method(RENDER_WIRE);
    set_cull_method(CULL_BACKFACE);

    init_light(vec3_new(0.0f, 0.0f, 1.0f));
    init_camera(vec3_new(0.0f, 0.0f, 0.0f), vec3_new(0.0f, 0.0f, 1.0f));

    float aspect_y = (float)get_window_height() / (float)get_window_width();
    float aspect_x = (float)get_window_width() / (float)get_window_height();
    float fov_y = M_PI / 3.0f;
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float z_near = 0.1f;
    float z_far = 100.0f;
    projection_matrix = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);

    init_frustrum_planes(fov_x, fov_y, z_near, z_far);

    load_mesh("../assets/meshes/f22.obj", "../assets/textures/f22.png", vec3_new(1.0f, 1.0f, 1.0f), vec3_new(-3.0f, 0.0f, 5.0f), vec3_new(0.0f, 0.0f, 0.0f));
    load_mesh("../assets/meshes/efa.obj", "../assets/textures/efa.png", vec3_new(1.0f, 1.0f, 1.0f), vec3_new(3.0f, 0.0f, 5.0f), vec3_new(0.0f, 0.0f, 0.0f));
}

void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
                break;
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                set_render_method(RENDER_WIRE_VERTEX);
                break;
            }
            if (event.key.keysym.sym == SDLK_2)
            {
                set_render_method(RENDER_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_3)
            {
                set_render_method(RENDER_FILL_TRIANGLE);
                break;
            }
            if (event.key.keysym.sym == SDLK_4)
            {
                set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_5)
            {
                set_render_method(RENDER_TEXTURED);
                break;
            }
            if (event.key.keysym.sym == SDLK_6)
            {
                set_render_method(RENDER_TEXTURED_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_c)
            {
                set_cull_method(CULL_BACKFACE);
                break;
            }
            if (event.key.keysym.sym == SDLK_x)
            {
                set_cull_method(CULL_NONE);
                break;
            }
            if (event.key.keysym.sym == SDLK_w)
            {
                update_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0f * delta_time));
                update_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
                break;
            }
            if (event.key.keysym.sym == SDLK_s)
            {
                update_camera_forward_velocity(vec3_mul(get_camera_direction(), -5.0f * delta_time));
                update_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
                break;
            }
            if (event.key.keysym.sym == SDLK_a)
            {
                rotate_camera_yaw(2.0f * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                rotate_camera_yaw(-2.0f * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_UP)
            {
                rotate_camera_pitch(2.0f * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_DOWN)
            {
                rotate_camera_pitch(-2.0f * delta_time);
                break;
            }
            break;
        default:
            break;
        }
    }
}


// Pipeline::Model Space => World Space => Camera Space => Clipping => Projection => Image Space => Screen Space
void process_graphics_pipeline_stages(mesh_t *mesh)
{
    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

    vec3_t target = get_camera_lookat_target();
    vec3_t up_direction = {0.0f, 1.0f, 0.0f};

    view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

    int num_faces = array_length(mesh->faces);

    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh->faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a];
        face_vertices[1] = mesh->vertices[mesh_face.b];
        face_vertices[2] = mesh->vertices[mesh_face.c];

        vec4_t transformed_vertices[3];

        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            world_matrix = mat4_identity();

            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t face_normal = get_triangle_normal(transformed_vertices);

        if (is_cull_backface())
        {
            vec3_t origin = {0.0f, 0.0f, 0.0f};
            vec3_t camera_ray = vec3_sub(origin, vec3_from_vec4(transformed_vertices[0]));

            float dot_normal_camera = vec3_dot(face_normal, camera_ray);

            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        polygon_t polygon = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv);
        clip_polygon(&polygon);

        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;
        triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

        for (int t = 0; t < num_triangles_after_clipping; t++)
        {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];

            vec4_t projected_points[3];

            for (int j = 0; j < 3; j++)
            {
                projected_points[j] = mat4_mul_vec4_projection(projection_matrix, triangle_after_clipping.points[j]);

                projected_points[j].x *= (get_window_width() / 2.0f);
                projected_points[j].y *= (get_window_height() / 2.0f);

                projected_points[j].y *= -1.0f;

                projected_points[j].x += (get_window_width() / 2.0f);
                projected_points[j].y += (get_window_height() / 2.0f);
            }

            float light_intensity_factor = -vec3_dot(face_normal, get_light_direction());

            uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

            triangle_t triangle_to_render =
                {
                    .points =
                        {
                            {projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
                            {projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
                            {projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}},
                    .color = triangle_color,
                    .texcoords =
                        {
                            {triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
                            {triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
                            {triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v},
                        },
                    .texture = mesh->texture,
                };

            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH)
            {
                triangles_to_render[num_triangles_to_render] = triangle_to_render;
                num_triangles_to_render++;
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

    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;

    previous_frame_time = SDL_GetTicks();

    num_triangles_to_render = 0;

    for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++)
    {
        mesh_t *mesh = get_mesh(mesh_index);

        mesh->rotation.x += 0.0f * delta_time;
        mesh->rotation.y += 0.0f * delta_time;
        mesh->rotation.z += 0.0f * delta_time;
        mesh->translation.z += 0.0f;

        process_graphics_pipeline_stages(mesh);
    }
}

void render(void)
{
    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    for (int i = 0; i < num_triangles_to_render; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (should_render_filled_triangles())
        {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color);
        }

        if (should_render_textured_triangles())
        {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                triangle.texture);
        }

        if (should_render_wireframe())
        {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color);
        }

        if (should_render_wire_vertex())
        {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }
    }

    render_color_buffer();
}

void free_resources()
{
    destroy_window();
    free_meshes();
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

    free_resources();

    return 0;
}