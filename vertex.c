#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "parser.h"
#include "raster.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier.glb>\n", argv[0]);
        return 1;
    }

    GLBData modelGeo = parse_glb_file(argv[1]);
    size_t sizeVer = modelGeo.unique_vertices_count;

    float rotA = 3.141592 / 2, rotB = 0, rotC = 0; // Rotation angles
    int lX = 60, lY = 40; // Buffer dimensions
    int scale = fmin(0.9 * lX / 2, 0.9 * lY);
    char buffer[lX * lY];
    float bufferZ[lX * lY];
    float D = 4; // Camera distance

    printf("\033[?25l"); // Hide cursor
    printf("\x1b[2J"); // Clear screen

    for (;;) {
        memset(buffer, 32, lX * lY);
        memset(bufferZ, 0, lX * lY * sizeof(float));

        float sinA = sin(rotA), cosA = cos(rotA);
        float sinB = sin(rotB), cosB = cos(rotB);
        float sinC = sin(rotC), cosC = cos(rotC);

        // Process triangles instead of individual vertices
        for (size_t triangle = 0; triangle < modelGeo.indices_count; triangle += 3) {
            Vertex v1 = modelGeo.unique_vertices[modelGeo.indices[triangle]];
            Vertex v2 = modelGeo.unique_vertices[modelGeo.indices[triangle + 1]];
            Vertex v3 = modelGeo.unique_vertices[modelGeo.indices[triangle + 2]];

            // Rotation X
            float y1 = v1.y * cosA - v1.z * sinA;
            float z1 = v1.y * sinA + v1.z * cosA;
            v1.y = y1; v1.z = z1;

            y1 = v2.y * cosA - v2.z * sinA;
            z1 = v2.y * sinA + v2.z * cosA;
            v2.y = y1; v2.z = z1;

            y1 = v3.y * cosA - v3.z * sinA;
            z1 = v3.y * sinA + v3.z * cosA;
            v3.y = y1; v3.z = z1;

            // Rotation Y
            float x2 = v1.x * cosB + v1.z * sinB;
            float z2 = -v1.x * sinB + v1.z * cosB;
            v1.x = x2; v1.z = z2;

            x2 = v2.x * cosB + v2.z * sinB;
            z2 = -v2.x * sinB + v2.z * cosB;
            v2.x = x2; v2.z = z2;

            x2 = v3.x * cosB + v3.z * sinB;
            z2 = -v3.x * sinB + v3.z * cosB;
            v3.x = x2; v3.z = z2;

            // Rotation Z
            float x3 = v1.x * cosC - v1.y * sinC;
            float y3 = v1.x * sinC + v1.y * cosC;
            v1.x = x3; v1.y = y3;

            x3 = v2.x * cosC - v2.y * sinC;
            y3 = v2.x * sinC + v2.y * cosC;
            v2.x = x3; v2.y = y3;

            x3 = v3.x * cosC - v3.y * sinC;
            y3 = v3.x * sinC + v3.y * cosC;
            v3.x = x3; v3.y = y3;

            // Project vertices
            int screenX1 = lX / 2 + 2 * (scale * v1.x / (D - v1.z));
            int screenY1 = lY / 2 + (scale * v1.y / (D - v1.z));
            float screenZ1 = D - v1.z;

            int screenX2 = lX / 2 + 2 * (scale * v2.x / (D - v2.z));
            int screenY2 = lY / 2 + (scale * v2.y / (D - v2.z));
            float screenZ2 = D - v2.z;

            int screenX3 = lX / 2 + 2 * (scale * v3.x / (D - v3.z));
            int screenY3 = lY / 2 + (scale * v3.y / (D - v3.z));
            float screenZ3 = D - v3.z;

            // Projection et dessin du triangle
            draw_triangle(buffer, bufferZ, screenX1, screenY1, screenZ1, screenX2, screenY2, screenZ2, screenX3, screenY3, screenZ3, lX, lY);
        }

        // Render buffer
        printf("\x1b[H");
        for (int i = 0; i < lX * lY; i++) {
            putchar(i % lX ? buffer[i] : 10);
        }
        
        // Update rotation
        rotA += 0.02;
        rotB += 0.03;
        //rotC += 0.01;
    }
    return 0;
}
