#include "nbody_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


void initialize_bodies(Body *bodies, int n) {
    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        bodies[i].x = ((float)rand() / RAND_MAX) * BOUND;
        bodies[i].y = ((float)rand() / RAND_MAX) * BOUND;
        bodies[i].z = ((float)rand() / RAND_MAX) * BOUND;
        bodies[i].vx = bodies[i].vy = bodies[i].vz = 0.0;
        bodies[i].mass = 1e14 + ((float)rand() / RAND_MAX) * (1e20 - 1e16);

        // printf("x: %f, y: %f, z: %f\n", bodies[i].x, bodies[i].y, bodies[i].z);
    }
}


double calculate_distance(Body a, Body b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}


// Not working with big amour of positions to write.
void write_position(Body *body) {
    FILE *file = fopen("../positions.txt", "a");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(file, "%.4f, %.4f, %.4f\n", body->x, body->y, body->z);
    fflush(file);

    // fclose(file);
}
