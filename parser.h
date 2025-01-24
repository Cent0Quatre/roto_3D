#ifndef GLB_PARSER_H
#define GLB_PARSER_H

#include <stddef.h>

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

// Prototypes des fonctions
int are_vertices_equal(Vertex v1, Vertex v2);
int find_or_add_vertex(Vertex* unique_vertices, size_t* count, Vertex new_vertex);
GLBData parse_glb_file(const char* filename);
void free_glb_data(GLBData* data);

#endif // GLB_PARSER_H
