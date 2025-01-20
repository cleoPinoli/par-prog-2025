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
        bodies[i].mass = MASS;

        // printf("x: %f, y: %f, z: %f\n", bodies[i].x, bodies[i].y, bodies[i].z);
    }
}


double calculate_distance(Body a, Body b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}
