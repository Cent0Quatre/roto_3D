#include <stdio.h>
#include <string.h>
#include <math.h>

// Structure représentant un vertex en 3D
typedef struct {
    float x, y, z;
} Vertex;

int main() {
    Vertex vertices[3] = {
        {1.0f, 2.0f, 3.0f},  // Vertex 1 (x = 1, y = 2, z = 3)
        {4.0f, 5.0f, 6.0f},  // Vertex 2 (x = 4, y = 5, z = 6)
        {0.0f, 3.0f, 2.0f}   // Vertex 3 (x = 7, y = 8, z = 9)
    };

    float rotA = 0, rotB = 0; // Angles de rotation
    int lX = 60, lY = 20; // Dimension du buffer
    int scale = fmin(0.68 * lX / 2, 0.68 * lY); // .68 ratio max pour rester dans le cadre entièrement; /2 car *2 plus x
    float angleX, angleY;
    char buffer[lX * lY];
    float bufferZ[lX * lY * 4];
    float cam_depth = 5.5; // distance de cam à l'objet

    printf("\033[?25l"); // Désactiver le curseur

    printf("\x1b[2J"); // Efface l'écran
    for (;;) {
        // A chaque génération
        memset(buffer, 32, lX * lY); // Initialise le buffer avec des espaces
        memset(bufferZ, 0, lX * lY * 4);  // Initialise le buffer de depth à 0

        for (int i = 0; i < 3; i++) {
            float px = vertices[i].x, py = vertices[i].y, pz = vertices[i].z;

            // centré sur le buffer
            int x = lX / 2 + 2 * ((px * cam_depth) / pz);
            int y = lY / 2 + (py * cam_depth) / pz;
            
            // produit vectoriel avec (0, 1, -1) notre source de lumière
            int bright = py - pz;
            int j = x + lX * y;
            // Si dans le buffer et si un point visible ("rangé le plus devant")
            if (lY > y && y > 0 && x > 0 && lX > x) {
                buffer[j] = ".,-~:;=!*#$@"[bright > 0 ? bright : 0];
            }
        }     

        printf("\x1b[H"); // Replace le curseur en haut à gauche
        for (int i = 0; lX * lY >= i; i++) {
            putchar(i % lX ? buffer[i] : 10); // Affiche le buffer
            rotA += 0.00002;
            rotB += 0.00002;
        }
    }
    return 0;
}
