#include "../nbody_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>


void print_time(int* rank, double* start_time, double* end_time);


/**
 * In this version the parallel execution is implemented in this way:
 * Every process initialize the bodies, but they all have the same values because each seed is the same.
 * So it is not necessary to share the bodies along the processes or with the root, because they all have the
 * same bodies and they can start the computation.
 */
int main(int argc, char **argv) {

    int n, steps;   // Default number of bodies and time steps.
    int rank;       // Rank of the processor, if you have 4 core, the rank will be from 0 to 3.
    int size;       // Total number of processors that you are going to use.

    double start_time, end_time;

    // If argc are less then  3, we start the default configuration, serial execution with 1000 body and 1000 steps.
    if (argc < 2) {
        n = steps = 1000;
    }

    n = atoi(argv[1]);      // The first argument n = (argv[1]).
    steps = atoi(argv[2]);  // The second argument steps = (argv[2]).

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("n-body simulation with bodies -> %d, steps -> %d.\n", n, steps);
    }

    // Allocate memory and initialize bodies.
    Body *bodies = (Body*)malloc(n * sizeof(Body));
    initialize_bodies(bodies, n, rank);

    if (argc >= 4) {
        // Octtree implementation.
        start_time = MPI_Wtime();

        if (argc == 4) {
            simulate_octtree(bodies, n, steps);
        } else {
            // simulate_par_octtree(bodies, n, steps, rank, size);
        }
        
        end_time = MPI_Wtime();

    } else if (size == 1) {
        // Serial execution.
        start_time = MPI_Wtime();
        simulate_serial(bodies, n, steps);
        end_time = MPI_Wtime();

    } else {
        // Parallel execution.
        start_time = MPI_Wtime();
        simulate_parallel(bodies, n, steps, rank, size);
        end_time = MPI_Wtime();
    }

    print_time(&rank, &start_time, &end_time);

    free(bodies);
    MPI_Finalize();

    return 0;
}



void print_time(int* rank, double* start_time, double* end_time) {
    if (*rank != 0) {
        return;
    }

    double elapsed_time = *end_time - *start_time;
    printf("Execution time: %.4f seconds.\n", elapsed_time);
}
