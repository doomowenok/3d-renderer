#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "UPNG/upng.h"

typedef struct 
{
    vec3_t* vertices;
    face_t* faces;
    upng_t* texture;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
} mesh_t;

void load_mesh(char* obj_filename, char* texture_filename, vec3_t scale, vec3_t translation, vec3_t rotation);
void load_mesh_obj_data(mesh_t* mesh, char* obj_filename);
void load_mesh_texture_data(mesh_t* mesh, char *texture_filename);

int get_num_meshes(void);
mesh_t* get_mesh(int index);

void free_meshes(void);

#endif