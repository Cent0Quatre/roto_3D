#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>

typedef struct {
    float x, y, z;
} Vertex;

int main() {
    Vertex vertices[8] = {
        {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
        {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
        {0, 0, 0}
    };

    float yP = 0;
    int lX = 60, lY = 30;
    int scale = fmin(0.68 * lX / 2, 0.68 * lY);
    char buffer[lX * lY];
    float bufferZ[lX * lY * 4];
    
    // Position fixe de la caméra
    float D = 4;  // Caméra reculée sur Z
    
    printf("\033[?25l");
    printf("\x1b[2J");

    for (;;) {
        memset(buffer, 32, lX * lY);
        memset(bufferZ, 0, lX * lY * 4);

        float sinY = sin(yP);
        float cosY = cos(yP);

        for (int i = 0; i < 8; i++) {
            // Rotation autour de Y avant projection
            float xP = vertices[i].x * cosY + vertices[i].z * sinY;
            float yP = vertices[i].y;
            float zP = -vertices[i].x * sinY + vertices[i].z * cosY;

            // Calcul du vecteur vers la caméra
            float dz = zP - D;
            
            // Projection perspective
            int x = lX / 2 + 2 * (int)(scale * xP / -dz);
            int y = lY / 2 + (int)(scale * yP / -dz);
            
            int j = x + lX * y;
            if (dz < 0 && y >= 0 && y < lY && x >= 0 && x < lX) {
                float depth = sqrtf(xP*xP + yP*yP + dz*dz);
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
        
        yP += 0.05;
        Sleep(10);
    }
    return 0;
}
