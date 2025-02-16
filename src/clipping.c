#include "clipping.h"
#include <math.h>

plane_t frustrum_planes[NUM_PLANES];

void init_frustrum_planes(float fov_x, float fov_y, float z_near, float z_far)
{
    float cos_half_fov_x = cos(fov_x / 2.0f);
    float sin_half_fov_x = sin(fov_x / 2.0f);

    float cos_half_fov_y = cos(fov_y / 2.0f);
    float sin_half_fov_y = sin(fov_y / 2.0f);

    frustrum_planes[LEFT_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos_half_fov_x;
    frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0.0f;
    frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin_half_fov_x;

    frustrum_planes[RIGHT_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.x = -cos_half_fov_x;
    frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.y = 0.0f;
    frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.z = sin_half_fov_x;

    frustrum_planes[TOP_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[TOP_FRUSTRUM_PLANE].normal.x = 0.0f;
    frustrum_planes[TOP_FRUSTRUM_PLANE].normal.y = -cos_half_fov_y;
    frustrum_planes[TOP_FRUSTRUM_PLANE].normal.z = sin_half_fov_y;

    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.x = 0.0f;
    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.y = cos_half_fov_y;
    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.z = sin_half_fov_y;

    frustrum_planes[NEAR_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, z_near);
    frustrum_planes[NEAR_FRUSTRUM_PLANE].normal.x = 0.0f;
    frustrum_planes[NEAR_FRUSTRUM_PLANE].normal.y = 0.0f;
    frustrum_planes[NEAR_FRUSTRUM_PLANE].normal.z = 1.0f;

    frustrum_planes[FAR_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, z_far);
    frustrum_planes[FAR_FRUSTRUM_PLANE].normal.x = 0.0f;
    frustrum_planes[FAR_FRUSTRUM_PLANE].normal.y = 0.0f;
    frustrum_planes[FAR_FRUSTRUM_PLANE].normal.z = -1.0f;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2)
{
    polygon_t polygon = 
    {
        .vecrtices = { v0, v1, v2 },
        .num_vertices = 3
    };

    return polygon;
}

void clip_polygon_against_plain(polygon_t* polygon, int plane)
{
    vec3_t plane_point = frustrum_planes[plane].point;
    vec3_t plane_normal = frustrum_planes[plane].normal;

    vec3_t inside_vertices[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    vec3_t* current_vertex = &polygon->vecrtices[0];
    vec3_t* previous_vertex = &polygon->vecrtices[polygon->num_vertices - 1];

    float current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);
    float previous_dot = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

    while(current_vertex != &polygon->vecrtices[polygon->num_vertices])
    {   
        current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

        if(current_dot * previous_dot < 0)
        {
            float t = previous_dot / (previous_dot - current_dot);

            vec3_t intersection_point = vec3_clone(current_vertex);
            intersection_point = vec3_sub(intersection_point, *previous_vertex);
            intersection_point = vec3_mul(intersection_point, t);
            intersection_point = vec3_add(intersection_point, *previous_vertex);

            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            num_inside_vertices++;
        }

        if(current_dot > 0)
        {
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            num_inside_vertices++;
        }

        previous_dot = current_dot;
        previous_vertex = current_vertex;
        current_vertex++;
    }

    for(int i = 0; i < num_inside_vertices; i++)
    {
        polygon->vecrtices[i] = vec3_clone(&inside_vertices[i]);
    }
    
    polygon->num_vertices = num_inside_vertices;

}

void clip_polygon(polygon_t* polygon)
{
    clip_polygon_against_plain(polygon, LEFT_FRUSTRUM_PLANE);
    clip_polygon_against_plain(polygon, RIGHT_FRUSTRUM_PLANE);
    clip_polygon_against_plain(polygon, TOP_FRUSTRUM_PLANE);
    clip_polygon_against_plain(polygon, BOTTOM_FRUSTRUM_PLANE);
    clip_polygon_against_plain(polygon, NEAR_FRUSTRUM_PLANE);
    clip_polygon_against_plain(polygon, FAR_FRUSTRUM_PLANE);
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles)
{
    for(int i = 0; i < polygon->num_vertices - 2; i++)
    {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(polygon->vecrtices[index0]);
        triangles[i].points[1] = vec4_from_vec3(polygon->vecrtices[index1]);
        triangles[i].points[2] = vec4_from_vec3(polygon->vecrtices[index2]);
    }

    *num_triangles = polygon->num_vertices - 2;
}