#include "matrix.h"

mat4_t mat4_identity(void)
{
    mat4_t m;

    m.m[0][0] = 1;
    m.m[0][1] = 0;
    m.m[0][2] = 0;
    m.m[0][3] = 0;

    m.m[1][0] = 0;
    m.m[1][1] = 1;
    m.m[1][2] = 0;
    m.m[1][3] = 0;

    m.m[2][0] = 0;
    m.m[2][1] = 0;
    m.m[2][2] = 1;
    m.m[2][3] = 0;

    m.m[3][0] = 0;
    m.m[3][1] = 0;
    m.m[3][2] = 0;
    m.m[3][3] = 1;

    return m;
}

mat4_t mat4_make_scale(float s_x, float s_y, float s_z)
{
    mat4_t m = mat4_identity();

    m.m[0][0] = s_x;
    m.m[1][1] = s_y;
    m.m[2][2] = s_z;

    return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
    vec4_t result;

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

    return result;
}
