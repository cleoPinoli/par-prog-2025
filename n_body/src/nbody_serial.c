#include "nbody_utils.h"

#include <stdio.h>
#include <math.h>


void simulate_serial(Body *bodies, int n, int steps) {
    FILE *file = fopen("../positions.txt", "a");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int step = 0; step < steps; step++) {

        for (int i = 0; i < n; i++) {
            double Fi = 0, Fx = 0, Fy = 0, Fz = 0;

            Body *body_i = &bodies[i];

            // Loop over all other bodies to compute forces.
            for (int j = 0; j < n; j++) {

                if (i == j) {
                    continue;
                }

                Body *body_j = &bodies[j];

                // Compute distance squared.
                double dist_sq = calculate_distance(*body_j, *body_i);
                double r = sqrt(dist_sq);

                // Compute gravitational force.
                double gravitational_force_ij = (G * body_i->mass * body_j->mass) / (dist_sq + EPS);

                // Compute differences in coordinates.
                double dx = body_j->x - body_i->x;
                double dy = body_j->y - body_i->y;
                double dz = body_j->z - body_i->z;

                // Sum all the forces to get back Fi += gravitational_force_ij.
                // Compute force for each direction.
                double fx = gravitational_force_ij * (dx / r);
                double fy = gravitational_force_ij * (dy / r);
                double fz = gravitational_force_ij * (dz / r);

                // TODO: check this if its correct.
                // Calculate the net force on body_i
                Fx += fx;
                Fy += fy;
                Fz += fz;
            }

            // TODO: check this with the todo above.
            // Calculate the acceleration -> ai = Fi / mi.
            double ai_x = Fx / body_i->mass;
            double ai_y = Fy / body_i->mass;
            double ai_z = Fz / body_i->mass;

            // Update velocity.
            body_i->vx += ai_x * DT;
            body_i->vy += ai_y * DT;
            body_i->vz += ai_z * DT;

            // Update position using Euler method
            body_i->x += body_i->vx * DT;
            body_i->y += body_i->vy * DT;
            body_i->z += body_i->vz * DT;

            // write_position(body_i);
            
            fprintf(file, "%.4f, %.4f, %.4f\n", body_i->x, body_i->y, body_i->z);
            fflush(file);
        }
    }

    fclose(file);
}
