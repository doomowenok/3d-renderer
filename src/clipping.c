#include "clipping.h"
#include <math.h>

plane_t frustrum_planes[NUM_PLANES];

void init_frustrum_planes(float fov, float z_near, float z_far)
{
    float cos_half_fov = cos(fov / 2.0f);
    float sin_half_fov = sin(fov / 2.0f);

    frustrum_planes[LEFT_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos_half_fov;
    frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0.0f;
    frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustrum_planes[RIGHT_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.x = -cos_half_fov;
    frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.y = 0.0f;
    frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustrum_planes[TOP_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[TOP_FRUSTRUM_PLANE].normal.x = 0.0f;
    frustrum_planes[TOP_FRUSTRUM_PLANE].normal.y = -cos_half_fov;
    frustrum_planes[TOP_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.x = 0.0f;
    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.y = cos_half_fov;
    frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.z = sin_half_fov;

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