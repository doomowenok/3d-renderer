#include <math.h>
#include "vector.h"

vec2_t vec2_new(float x, float y)
{
    vec2_t result = { x, y };
    return result;
}

float vec2_length(vec2_t v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t v0, vec2_t v1)
{
    vec2_t result = { .x = v0.x + v1.x, .y = v0.y + v1.y };
    return result;
}

vec2_t vec2_sub(vec2_t v0, vec2_t v1)
{
    vec2_t result = { .x = v0.x - v1.x, .y = v0.y - v1.y };
    return result;
}

vec2_t vec2_mul(vec2_t v, float factor)
{
    vec2_t result = { .x = v.x * factor, .y = v.y * factor };
    return result;
}

vec2_t vec2_div(vec2_t v, float factor)
{
    vec2_t result = { .x = v.x / factor, .y = v.y / factor };
    return result;
}

float vec2_dot(vec2_t v0, vec2_t v1)
{
    return v0.x * v1.x + v0.y * v1.y;
}

void vec2_normalize(vec2_t* v)
{
    float length = vec2_length(*v);
    v->x /= length;
    v->y /= length;
}

vec3_t vec3_new(float x, float y, float z)
{
    vec3_t result = { x, y, z };
    return result;
}

vec3_t vec3_clone(vec3_t* v)
{
    vec3_t result = { v->x, v->y, v->z };
    return result;
}

float vec3_length(vec3_t v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t v0, vec3_t v1)
{
    vec3_t result = { .x = v0.x + v1.x, .y = v0.y + v1.y, .z = v0.z + v1.z };
    return result;
}

vec3_t vec3_sub(vec3_t v0, vec3_t v1)
{
    vec3_t result = { .x = v0.x - v1.x, .y = v0.y - v1.y, .z = v0.z - v1.z };
    return result;
}

vec3_t vec3_mul(vec3_t v, float factor)
{
    vec3_t result = { .x = v.x * factor, .y = v.y * factor, .z = v.z * factor };
    return result;
}

vec3_t vec3_div(vec3_t v, float factor)
{
    vec3_t result = { .x = v.x / factor, .y = v.y / factor, .z = v.z / factor };
    return result;
}

vec3_t vec3_cross(vec3_t v0, vec3_t v1)
{
    vec3_t result = 
    {
        .x = v0.y * v1.z - v0.z * v1.y,
        .y = v0.z * v1.x - v0.x * v1.z,
        .z = v0.x * v1.y - v0.y * v1.x
    };
    return result;
}

float vec3_dot(vec3_t v0, vec3_t v1)
{
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

void vec3_normalize(vec3_t* v)
{
    float length = vec3_length(*v);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

vec3_t vec3_rotate_x(vec3_t v, float angle)
{
    vec3_t rotated_vector = 
    {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle)
{
    vec3_t rotated_vector = 
    {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle)
{
    vec3_t rotated_vector = 
    {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}

vec4_t vec4_from_vec3(vec3_t v)
{
    vec4_t result = 
    {
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = 1.0f
    };

    return result;
}

vec3_t vec3_from_vec4(vec4_t v)
{
    vec3_t result = 
    {
        .x = v.x,
        .y = v.y,
        .z = v.z
    };

    return result;
}

vec2_t vec2_from_vec4(vec4_t v)
{
    vec2_t result = 
    {
        .x = v.x,
        .y = v.y
    };

    return result;
}
