#include <stdio.h>
#include <string.h>
#include <math.h>
#include "parser.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier.glb>\n", argv[0]);
        return 1;
    }

    GLBData modelGeo = parse_glb_file(argv[1]);
    size_t vertices = modelGeo.unique_vertices_count;

    float rotA = 0, rotB = 0, rotC = 0; // Angles de rotation
    int lX = 50, lY = 20; // Dimensions du buffer
    int scale = fmin(0.40 * lX / 2, 0.40 * lY);
    char buffer[lX * lY];
    float bufferZ[lX * lY * 4];
    float D = 3; // Distance de l'objet à la caméra
    
    printf("\033[?25l"); // Désactive le curseur

    printf("\x1b[2J"); // Efface l'écran
    for (;;) {
        // A chaque génération
        memset(buffer, 32, lX * lY);
        memset(bufferZ, 0, lX * lY * 4);

        float sinA = sin(rotA), cosA = cos(rotA);
        float sinB = sin(rotB), cosB = cos(rotB);
        float sinC = sin(rotC), cosC = cos(rotC);

        for (int i = 0; i < vertices; i++) {
            float x = modelGeo.unique_vertices[i].x, y = modelGeo.unique_vertices[i].y, z = modelGeo.unique_vertices[i].z;

            // Rotation X
            float y1 = y * cosA - z * sinA;
            float z1 = y * sinA + z * cosA;
            y = y1;
            z = z1;

            // Rotation Y
            float x2 = x * cosB + z * sinB;
            float z2 = -x * sinB + z * cosB;
            x = x2;
            z = z2;

            // Rotation Z
            float x3 = x * cosC - y * sinC;
            float y3 = x * sinC + y * cosC;
            x = x3;
            y = y3;

            int screenX = lX / 2 + 2 * (scale * x / (D - z));
            int screenY = lY / 2 + (scale * y / (D - z));
            
            int j = screenX + lX * screenY;
            // Si le point est dans le cadre du buffer
            if (z-D < 0 && screenY >= 0 && screenY < lY && screenX >= 0 && screenX < lX) {
                float depth = sqrtf(x*x + y*y + (z-D)*(z-D));
                if (depth < bufferZ[j] || bufferZ[j] == 0) {
                    bufferZ[j] = depth;
                    int bright = 11 - (int)(depth);
                    if (bright < 0) bright = 0;
                    if (bright > 11) bright = 11;
                    buffer[j] = ".,-~:;=!*#$@"[bright];
                }
            }
        }

        printf("\x1b[H");
        for (int i = 0; i < lX * lY; i++) {
            putchar(i % lX ? buffer[i] : 10);
        }
        
        rotA += 0.02;
        rotB += 0.03;
        rotC += 0.01;
    }
    return 0;
}
