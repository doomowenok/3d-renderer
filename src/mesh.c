#include <stdio.h>
#include "mesh.h"
#include "DynamicArray/array.h"

mesh_t mesh = 
{ 
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 },
    .scale = { 1.0f, 1.0f, 1.0f },
    .translation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] =
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

face_t cube_faces[N_CUBE_FACES] =
{
    // Front
    {.a = 1, .b = 2, .c = 3, .color = 0xFFFFFF00},
    {.a = 1, .b = 3, .c = 4, .color = 0xFFFFFF00},

    // Right  
    {.a = 4, .b = 3, .c = 5, .color = 0xFFFF00FF},
    {.a = 4, .b = 5, .c = 6, .color = 0xFFFF00FF},

    // Back
    {.a = 6, .b = 5, .c = 7, .color = 0xFF00FFFF},
    {.a = 6, .b = 7, .c = 8, .color = 0xFF00FFFF},

    // Left
    {.a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00},
    {.a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00},
 
    // Top
    {.a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF},
    {.a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF},

    // Bottom
    {.a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF},
    {.a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF},
};

void load_obj_file_data(char* filename)
{
    FILE* file = NULL;
    char line[1024];

    file = fopen(filename, "r");

    if (!file)
    {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    while(fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == 'v')
        {
            vec3_t vertex = { 0.0f, 0.0f, 0.0f };

            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);

            array_push(mesh.vertices, vertex);
        }
        else if (line[0] == 'f')
        {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            
            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0], 
                &vertex_indices[1], &texture_indices[1], &normal_indices[1], 
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            ); 

            face_t face = {
                .a = vertex_indices[0],
                .b = vertex_indices[1],
                .c = vertex_indices[2]
            };

            array_push(mesh.faces, face);
        }
    }

    fclose(file);
}

void load_cube_mesh_data() 
{
    for(int i = 0; i < N_CUBE_VERTICES; i++)
    {
        array_push(mesh.vertices, cube_vertices[i]);
    }

    for(int i = 0; i < N_CUBE_FACES; i++)
    {
        array_push(mesh.faces, cube_faces[i]);
    }
}