#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"

#define NUM_PLANES 6

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

extern plane_t frustrum_planes[NUM_PLANES];

void init_frustrum_planes(float fov, float z_near, float z_far);

#endif