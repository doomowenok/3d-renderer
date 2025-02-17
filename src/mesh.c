#include <stdio.h>
#include "mesh.h"
#include "DynamicArray/array.h"

#define MAX_MESHES 10

static mesh_t meshes[MAX_MESHES];
static int mesh_count = 0;

void load_mesh(char* obj_filename, char *texture_filename, vec3_t scale, vec3_t translation, vec3_t rotation)
{
    load_mesh_obj_data(&meshes[mesh_count], obj_filename);
    load_mesh_texture_data(&meshes[mesh_count], texture_filename);
    
    meshes[mesh_count].scale = scale;
    meshes[mesh_count].translation = translation;
    meshes[mesh_count].rotation = rotation;
    
    mesh_count++;
}

void load_mesh_obj_data(mesh_t* mesh, char* obj_filename)
{
    FILE* file = NULL;
    char line[1024];

    file = fopen(obj_filename, "r");

    if (!file)
    {
        printf("Failed to open file: %s\n", obj_filename);
        return;
    }

    tex2_t* texcoords = NULL;

    while(fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            vec3_t vertex = { 0.0f, 0.0f, 0.0f };

            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);

            array_push(mesh->vertices, vertex);
        }

        if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        }
        
        if (line[0] == 'f')
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

            // -1 because indexes in .OBJ starts from 1
            face_t face = 
            {
                .a = vertex_indices[0] - 1,
                .b = vertex_indices[1] - 1,
                .c = vertex_indices[2] - 1,
                .a_uv = texcoords[texture_indices[0] - 1],
                .b_uv = texcoords[texture_indices[1] - 1],
                .c_uv = texcoords[texture_indices[2] - 1],
                .color = 0xFFFFFFFF
            };

            array_push(mesh->faces, face);
        }
    }

    fclose(file);
    array_free(texcoords);
}

void load_mesh_texture_data(mesh_t* mesh, char* texture_filename)
{
    upng_t* png_texture = upng_new_from_file(texture_filename);

    if(png_texture != NULL)
    {
        upng_decode(png_texture);

        if(upng_get_error(png_texture) == UPNG_EOK)
        {
            mesh->texture = png_texture;
        }
    }
}

int get_num_meshes(void)
{
    return mesh_count;
}

mesh_t* get_mesh(int index)
{
    return &meshes[index];
}

void free_meshes(void)
{
    for (int i = 0; i < mesh_count; i++)
    {
        mesh_t* mesh = get_mesh(i);
        array_free(mesh->faces);
        array_free(mesh->vertices);
        upng_free(mesh->texture);
    }
}