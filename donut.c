#include <stdio.h>
#include <string.h>
#include <math.h>

int main() {
    float rotA = 0, rotB = 0; // Angles de rotation
    float angleX, angleY;
    char buffer[1760];
    float bufferZ[1760];

    printf("\033[?25l"); // Désactiver le curseur

    printf("\x1b[2J"); // Efface l'écran
    for (;;) {
        memset(buffer, 32, 1760); // Initialise le buffer avec des espaces
        memset(bufferZ, 0, 7040);  // Initialise le buffer de depth à 0

        for (angleY = 0; 6.28 > angleY; angleY += 0.07) { // Boucle sur l'angle vertical
            for (angleX = 0; 6.28 > angleX; angleX += 0.02) { // Boucle sur l'angle horizontal
                float sinX = sin(angleX), sinY = sin(angleY), cosX = cos(angleX), cosY = cos(angleY);
                float sinA = sin(rotA), cosA = cos(rotA), sinB = sin(rotB), cosB = cos(rotB);
                float ecart = cosY + 2; // centre non nul pour ne pas diviser par 0
                
                float depth = sinX * ecart * cosA - sinY * sinA;
                float cam_depth = 3.5; // distance de l'objet
                float persepective = sinX * ecart * sinA + sinY * cosA + cam_depth;
                // 40 et 12: milieu de l'écran
                // 30 et 15: échelle de projection
                int x = 40 + 30 * (cosX * ecart * cosB - depth * sinB) / persepective;
                int y = 12 + 15 * (cosX * ecart * sinB + depth * cosB) / persepective;
                
                int bright = 8 * ((sinY * sinA - sinX * cosY * cosA) * cosB - sinX * cosY * sinA - sinY * cosA - cosX * cosY * sinB);
                int j = x + 80 * y;
                // Si dans le buffer et si un point visible ("rangeé le plus devant")
                if (22 > y && y > 0 && x > 0 && 80 > x && 1 / persepective > bufferZ[j]) {
                    bufferZ[j] = 1 / persepective;
                    buffer[j] = ".,-~:;=!*#$@"[bright > 0 ? bright : 0];
                }
            }
        }

        printf("\x1b[H"); // Replace le curseur en haut à gauche
        for (int i = 0; 1761 > i; i++) {
            putchar(i % 80 ? buffer[i] : 10); // Affiche le buffer
            rotA += 0.00002;
            rotB += 0.00002;
        }
    }
    return 0;
}
