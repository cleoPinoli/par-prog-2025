#ifndef NBODY_UTILS_H
#define NBODY_UTILS_H


#define G 6.67430e-11   // Gravitational constant
#define DT 0.01         // Time step
#define EPS 1e-9        // Softening factor to avoid singularities
#define MASS 1.899e12   //  for m.


typedef struct {
    double x, y, z;  // Position
    double vx, vy, vz;  // Velocity
    double mass;     // Mass
} Body;


void initialize_bodies(Body *bodies, int n);
double calculate_distance(Body a, Body b);
void simulate_serial(Body *bodies, int n, int steps);
void simulate_parallel(Body *bodies, int n, int steps, int rank, int size);

#endif
