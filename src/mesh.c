#include "mesh.h"

vec3_t mesh_vertices[N_MESH_VERTICES] =
{
    {.x = -1.0f, .y = -1.0f, .z = -1.0f},
    {.x = -1.0f, .y = 1.0f, .z = -1.0f},
    {.x = 1.0f, .y = 1.0f, .z = -1.0f},
    {.x = 1.0f, .y = -1.0f, .z = -1.0f},
    {.x = 1.0f, .y = 1.0f, .z = 1.0f},
    {.x = 1.0f, .y = -1.0f, .z = 1.0f},
    {.x = -1.0f, .y = 1.0f, .z = 1.0f},
    {.x = -1.0f, .y = -1.0f, .z = 1.0f},
};

face_t mesh_faces[N_MESH_FACES] =
{
    // Front
    {.a = 1, .b = 2, .c = 3},
    {.a = 1, .b = 3, .c = 4},

    // Right
    {.a = 4, .b = 3, .c = 5},
    {.a = 4, .b = 5, .c = 6},

    // Back
    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},

    // Left
    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},

    // Top
    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},

    // Bottom
    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4},
};