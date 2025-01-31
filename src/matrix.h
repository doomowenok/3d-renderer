#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct 
{
    float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float s_x, float s_y, float s_z);
mat4_t mat4_make_translation(float t_x, float t_y, float t_z);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

#endif