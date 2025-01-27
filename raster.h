#ifndef RASTER_H
#define RASTER_H

// Fonction d'échange de valeurs entre deux entiers
void swap(int* a, int* b);

// Fonction de dessin de triangle pour rendu 2D
void draw_triangle(
    char* buffer,        // Buffer de caractères pour le rendu
    float* bufferZ,      // Buffer de profondeur 
    int x1, int y1, float z1,  // Coordonnées du premier sommet
    int x2, int y2, float z2,  // Coordonnées du deuxième sommet
    int x3, int y3, float z3,  // Coordonnées du troisième sommet
    int lX, int lY       // Dimensions du buffer
);

#endif // TRIANGLE_UTILS_H
