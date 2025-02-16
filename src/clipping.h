#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"
#include "triangle.h"

#define NUM_PLANES 6
#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLES 10

enum
{
    LEFT_FRUSTRUM_PLANE,
    RIGHT_FRUSTRUM_PLANE,
    TOP_FRUSTRUM_PLANE,
    BOTTOM_FRUSTRUM_PLANE,
    NEAR_FRUSTRUM_PLANE,
    FAR_FRUSTRUM_PLANE,
};

typedef struct
{
    vec3_t point;
    vec3_t normal;
} plane_t;

typedef struct
{
    vec3_t vecrtices[MAX_NUM_POLY_VERTICES];
    tex2_t texcoords[MAX_NUM_POLY_VERTICES];
    int num_vertices;
} polygon_t;

void init_frustrum_planes(float fov_x, float fov_y, float z_near, float z_far);
polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2);
void clip_polygon(polygon_t* polygon);
void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);

#endif