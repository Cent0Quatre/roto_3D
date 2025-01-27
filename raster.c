#include "raster.h"
#include <stdio.h>

// Fonction utilitaire pour échanger deux entiers
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void draw_triangle(char* buffer, float* bufferZ, int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, int lX, int lY) {
    // Vérification des limites de l'écran
    if (x1 < 0 || x1 >= lX || x2 < 0 || x2 >= lX || x3 < 0 || x3 >= lX || 
        y1 < 0 || y1 >= lY || y2 < 0 || y2 >= lY || y3 < 0 || y3 >= lY) {
        return;
    }

    // Tri des sommets par coordonnée y
    if (y1 > y2) {
        swap(&x1, &x2); swap(&y1, &y2); 
        float tempz = z1; z1 = z2; z2 = tempz;
    }
    if (y1 > y3) {
        swap(&x1, &x3); swap(&y1, &y3);
        float tempz = z1; z1 = z3; z3 = tempz;
    }
    if (y2 > y3) {
        swap(&x2, &x3); swap(&y2, &y3);
        float tempz = z2; z2 = z3; z3 = tempz;
    }

    // Calcul des pentes
    float slope1 = (x2 - x1) / (float)(y2 - y1 + 1e-8);
    float slope2 = (x3 - x1) / (float)(y3 - y1 + 1e-8);
    float slope3 = (x3 - x2) / (float)(y3 - y2 + 1e-8);

    float zSlope1 = (z2 - z1) / (float)(y2 - y1 + 1e-8);
    float zSlope2 = (z3 - z1) / (float)(y3 - y1 + 1e-8);
    float zSlope3 = (z3 - z2) / (float)(y3 - y2 + 1e-8);

    // Vecteur normal du triangle
    float uX = x2 - x1, uY = y2 - y1, uZ = z2 - z1;
    float vX = x3 - x1, vY = y3 - y1, vZ = z3 - z1;
    
    // Produit vectoriel pour la normale
    float Nx = (uY * vZ) - (uZ * vY);
    float Ny = (uZ * vX) - (uX * vZ);
    float Nz = (uX * vY) - (uY * vX);
    
    // Normalisation du vecteur normal
    float norm = sqrt(Nx*Nx + Ny*Ny + Nz*Nz);
    if (norm > 1e-8) {
        Nx /= norm;
        Ny /= norm;
        Nz /= norm;
    }

    // Direction de la lumière (normalisée)
    float Lx = 0.0f, Ly = 0.0f, Lz = -1.0f;  // Lumière venant de face
    
    // Produit scalaire pour l'intensité lumineuse
    float dot = -(Nx*Lx + Ny*Ly + Nz*Lz);
    // Remap [-1,1] to [0.2,1] pour garantir une luminosité minimale
    float baseIntensity = (dot + 1.0f) * 0.4f + 0.2f;

    // Dessin des triangles en deux parties
    float curX1, curX2, curZ1, curZ2;
    // Ajout d'un point pour les surfaces très sombres
    char luminanceChars[] = ".,-~:;=!*#$@";
    int numChars = strlen(luminanceChars);
    
    // Partie supérieure du triangle
    for (int y = y1; y <= y2; y++) {
        curX1 = x1 + (y - y1) * slope1;
        curX2 = x1 + (y - y1) * slope2;
        curZ1 = z1 + (y - y1) * zSlope1;
        curZ2 = z1 + (y - y1) * zSlope2;

        if (curX1 > curX2) {
            swap((int*)&curX1, (int*)&curX2);
            float tempz = curZ1; curZ1 = curZ2; curZ2 = tempz;
        }

        for (int x = (int)curX1; x <= (int)curX2; x++) {
            float t = (x - curX1) / (curX2 - curX1 + 1e-8);
            float depth = curZ1 + t * (curZ2 - curZ1);
            
            int index = x + lX * y;
            
            if (index >= 0 && index < lX * lY && (depth < bufferZ[index] || bufferZ[index] == 0)) {
                bufferZ[index] = depth;
                
                // Calcul amélioré de la luminance avec valeur minimale
                float depthFactor = 1.0f - (depth / 12.0f);
                depthFactor = depthFactor < 0.2f ? 0.2f : (depthFactor > 1.0f ? 1.0f : depthFactor);
                
                float finalIntensity = baseIntensity * depthFactor;
                finalIntensity = pow(finalIntensity, 0.8f);  // Correction gamma
                // Garantir une intensité minimale
                finalIntensity = finalIntensity < 0.1f ? 0.1f : finalIntensity;
                
                int bright = (int)(finalIntensity * (numChars - 1));
                bright = (bright < 0) ? 0 : (bright >= numChars ? numChars - 1 : bright);
                
                buffer[index] = luminanceChars[bright];
            }
        }
    }

    // Partie inférieure du triangle
    for (int y = y2 + 1; y <= y3; y++) {
        curX1 = x2 + (y - y2) * slope3;
        curX2 = x1 + (y - y1) * slope2;
        curZ1 = z2 + (y - y2) * zSlope3;
        curZ2 = z1 + (y - y1) * zSlope2;

        if (curX1 > curX2) {
            swap((int*)&curX1, (int*)&curX2);
            float tempz = curZ1; curZ1 = curZ2; curZ2 = tempz;
        }

        for (int x = (int)curX1; x <= (int)curX2; x++) {
            float t = (x - curX1) / (curX2 - curX1 + 1e-8);
            float depth = curZ1 + t * (curZ2 - curZ1);
            
            int index = x + lX * y;
            
            if (index >= 0 && index < lX * lY && (depth < bufferZ[index] || bufferZ[index] == 0)) {
                bufferZ[index] = depth;
                
                // Calcul amélioré de la luminance avec valeur minimale
                float depthFactor = 1.0f - (depth / 12.0f);
                depthFactor = depthFactor < 0.2f ? 0.2f : (depthFactor > 1.0f ? 1.0f : depthFactor);
                
                float finalIntensity = baseIntensity * depthFactor;
                finalIntensity = pow(finalIntensity, 0.8f);  // Correction gamma
                // Garantir une intensité minimale
                finalIntensity = finalIntensity < 0.1f ? 0.1f : finalIntensity;
                
                int bright = (int)(finalIntensity * (numChars - 1));
                bright = (bright < 0) ? 0 : (bright >= numChars ? numChars - 1 : bright);
                
                buffer[index] = luminanceChars[bright];
            }
        }
    }
}
