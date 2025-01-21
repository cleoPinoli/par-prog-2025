#include "nbody_utils.h"

#include <stdio.h>
#include <math.h>
#include <mpi.h>


void simulate_parallel(Body *bodies, int n, int steps, int rank, int size) {
    
    FILE *file;

    if (rank == 0) {
        file = fopen("../positions.txt", "a");

        if (file == NULL) {
            printf("Error opening file!\n");
            return;
        }
    }

    int chunk_size = n / size;
    int start = rank * chunk_size;
    int end = (rank == size - 1) ? n : start + chunk_size;

    for (int step = 0; step < steps; step++) {

        for (int i = start; i < end; i++) {
            double fx = 0, fy = 0, fz = 0;

            for (int j = 0; j < n; j++) {

                if (i == j) {
                    continue;
                }

                double r = calculate_distance(bodies[i], bodies[j]);
                double f = (G * bodies[i].mass * bodies[j].mass) / (r + EPS);
                fx += f * (bodies[j].x - bodies[i].x) / r;
                fy += f * (bodies[j].y - bodies[i].y) / r;
                fz += f * (bodies[j].z - bodies[i].z) / r;
            }

            bodies[i].vx += fx / bodies[i].mass * DT;
            bodies[i].vy += fy / bodies[i].mass * DT;
            bodies[i].vz += fz / bodies[i].mass * DT;

            bodies[i].x += bodies[i].vx * DT;
            bodies[i].y += bodies[i].vy * DT;
            bodies[i].z += bodies[i].vz * DT;
        }

        MPI_Allgather(MPI_IN_PLACE, chunk_size * sizeof(Body), MPI_BYTE,
                      bodies, chunk_size * sizeof(Body), MPI_BYTE, MPI_COMM_WORLD);

        if (rank == 0) {
            for (int i = 0; i < n; i++) {
                fprintf(file, "%.4f, %.4f, %.4f\n", bodies[i].x, bodies[i].y, bodies[i].z);
                fflush(file);
            }
        }
    }

    if (rank == 0) fclose(file);
}
