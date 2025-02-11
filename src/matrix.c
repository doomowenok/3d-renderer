#include "matrix.h"
#include <math.h>

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

mat4_t mat4_make_translation(float t_x, float t_y, float t_z)
{
    mat4_t m = mat4_identity();

    m.m[0][3] = t_x;
    m.m[1][3] = t_y;
    m.m[2][3] = t_z;

    return m;
}

mat4_t mat4_make_rotation_z(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();

    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;

    return m;
}

mat4_t mat4_make_rotation_x(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();

    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;

    return m;
}

mat4_t mat4_make_rotation_y(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();

    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;

    return m;
}

vec4_t mat4_mul_vec4_projection(mat4_t projection_matrix, vec4_t v)
{
    vec4_t result = mat4_mul_vec4(projection_matrix, v);

    if(result.w != 0.0f)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return result;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up)
{
    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z);
    vec3_t x = vec3_cross(up, z);
    vec3_normalize(&x);
    vec3_t y = vec3_cross(z, x);
    vec3_normalize(&y);

    mat4_t view_matrix = mat4_identity();
    
    view_matrix.m[0][0] = x.x;
    view_matrix.m[0][1] = x.y;
    view_matrix.m[0][2] = x.z;
    view_matrix.m[0][3] = -vec3_dot(x, eye);

    view_matrix.m[1][0] = y.x;
    view_matrix.m[1][1] = y.y;
    view_matrix.m[1][2] = y.z;
    view_matrix.m[1][3] = -vec3_dot(y, eye);

    view_matrix.m[2][0] = z.x;
    view_matrix.m[2][1] = z.y;
    view_matrix.m[2][2] = z.z;
    view_matrix.m[2][3] = -vec3_dot(z, eye);

    return view_matrix;
}

mat4_t mat4_make_perspective(float fov, float aspect, float z_near, float z_far)
{
    mat4_t m = {{{ 0 }}};
    m.m[0][0] = aspect * (1 / tan(fov / 2));
    m.m[1][1] = 1 / tan(fov / 2);
    m.m[2][2] = z_far / (z_far - z_near);
    m.m[2][3] = (-z_far * z_near) / (z_far - z_near);
    m.m[3][2] = 1.0;
    return m;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b)
{
    mat4_t m;

    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }

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
