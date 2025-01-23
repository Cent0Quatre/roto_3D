#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#define EPSILON 0.00001
#define MAX_VERTICES 1000

// Structure pour stocker les vertices uniques
typedef struct {
    float x, y, z;
} Vertex;

// Structure pour stocker les données du modèle
typedef struct {
    Vertex* unique_vertices;
    size_t unique_vertices_count;
    unsigned int* indices;
    size_t indices_count;
} GLBData;

// Fonction pour vérifier si deux vertices sont presque identiques
int are_vertices_equal(Vertex v1, Vertex v2) {
    return (fabs(v1.x - v2.x) < EPSILON &&
            fabs(v1.y - v2.y) < EPSILON &&
            fabs(v1.z - v2.z) < EPSILON);
}

// Trouver ou ajouter un vertex unique
int find_or_add_vertex(Vertex* unique_vertices, size_t* count, Vertex new_vertex) {
    for (size_t i = 0; i < *count; i++) {
        if (are_vertices_equal(unique_vertices[i], new_vertex)) {
            return i;  // Vertex déjà existant
        }
    }
    
    // Ajouter un nouveau vertex unique
    unique_vertices[*count] = new_vertex;
    return (*count)++;
}

GLBData parse_glb_file(const char* filename) {
    GLBData result = {0};
    
    // Allouer de la mémoire pour les vertices uniques et indices
    result.unique_vertices = malloc(MAX_VERTICES * sizeof(Vertex));
    result.indices = malloc(MAX_VERTICES * 3 * sizeof(unsigned int));
    
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    
    // Parser le fichier
    cgltf_result parsing_result = cgltf_parse_file(&options, filename, &data);
    if (parsing_result != cgltf_result_success) {
        fprintf(stderr, "Erreur lors du parsing du fichier GLB\n");
        return result;
    }
    
    // Charger les buffers
    parsing_result = cgltf_load_buffers(&options, data, filename);
    if (parsing_result != cgltf_result_success) {
        fprintf(stderr, "Erreur de chargement des buffers\n");
        cgltf_free(data);
        return result;
    }
    
    // Vérifier les maillages
    if (!data->meshes || data->meshes_count == 0) {
        fprintf(stderr, "Aucun maillage trouvé\n");
        cgltf_free(data);
        return result;
    }
    
    // Prendre le premier maillage
    cgltf_mesh* mesh = &data->meshes[0];
    cgltf_primitive* primitive = &mesh->primitives[0];
    
    // Trouver l'accesseur de position
    cgltf_accessor* pos_accessor = NULL;
    for (size_t i = 0; i < primitive->attributes_count; i++) {
        if (primitive->attributes[i].type == cgltf_attribute_type_position) {
            pos_accessor = primitive->attributes[i].data;
            break;
        }
    }
    
    if (!pos_accessor) {
        fprintf(stderr, "Aucun attribut de position trouvé\n");
        cgltf_free(data);
        return result;
    }
    
    // Extraire et dédupliquer les vertices
    for (size_t i = 0; i < pos_accessor->count; i++) {
        Vertex v;
        cgltf_accessor_read_float(pos_accessor, i, &v.x, 3);
        
        // Trouver ou ajouter le vertex unique
        int new_index = find_or_add_vertex(result.unique_vertices, 
                                           &result.unique_vertices_count, 
                                           v);
    }
    
    // Traiter les indices
    if (primitive->indices) {
        for (size_t i = 0; i < primitive->indices->count; i++) {
            unsigned int original_index = cgltf_accessor_read_index(primitive->indices, i);
            
            // Lire le vertex original
            Vertex v;
            cgltf_accessor_read_float(pos_accessor, original_index, &v.x, 3);
            
            // Trouver l'index du vertex unique
            for (size_t j = 0; j < result.unique_vertices_count; j++) {
                if (are_vertices_equal(result.unique_vertices[j], v)) {
                    result.indices[result.indices_count++] = j;
                    break;
                }
            }
        }
    }
    
    // Libérer les ressources de cgltf
    cgltf_free(data);
    
    return result;
}

// Fonction pour libérer la mémoire
void free_glb_data(GLBData* data) {
    free(data->unique_vertices);
    free(data->indices);
    data->unique_vertices = NULL;
    data->indices = NULL;
    data->unique_vertices_count = 0;
    data->indices_count = 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier.glb>\n", argv[0]);
        return 1;
    }
    
    GLBData parsed_data = parse_glb_file(argv[1]);
    
    // Afficher les vertices uniques
    printf("Vertices Uniques (%zu):\n", parsed_data.unique_vertices_count);
    for (size_t i = 0; i < parsed_data.unique_vertices_count; i++) {
        printf("  Vertex %zu: (%.6f, %.6f, %.6f)\n", 
               i,
               parsed_data.unique_vertices[i].x, 
               parsed_data.unique_vertices[i].y, 
               parsed_data.unique_vertices[i].z);
    }
    
    // Afficher les indices réactualisés
    printf("\nIndices (%zu):\n", parsed_data.indices_count);
    for (size_t i = 0; i < parsed_data.indices_count; i++) {
        printf("%u ", parsed_data.indices[i]);
        if ((i + 1) % 3 == 0) printf("\n");
    }
    
    // Libérer la mémoire
    free_glb_data(&parsed_data);
    
    return 0;
}
