#ifndef NBODY_UTILS_H
#define NBODY_UTILS_H

#define G 6.67430e-11   // Gravitational constant
#define DT 0.05         // Time step
#define EPS 1e-9        // Softening factor to avoid singularities
#define MASS 5.972e24   //  for m.
#define BOUND 2000      // Max size for x, y, z.
#define DIM 3           // Cube dimension, 3 in a 3D space.
#define THETA 0.5       // 


typedef struct {
    double x, y, z;  // Position
    double vx, vy, vz;  // Velocity
    double mass;     // Mass
} Body;


void initialize_bodies(Body *bodies, int n, int rank);
double calculate_distance(Body a, Body b);
void simulate_serial(Body *bodies, int n, int steps);
void simulate_parallel(Body *bodies, int n, int steps, int rank, int size);
void simulate_octtree(Body *bodies, int n, int steps);
// void simulate_par_octtree(Body *bodies, int n, int steps, int rank, int size);


typedef struct OctreeNode {
    double center[DIM];
    double half_size;
    double mass;
    double center_of_mass[DIM];
    Body *body;
    struct OctreeNode *children[8];
} OctreeNode;


OctreeNode *create_octree_node(double center[], double half_size);
void insert_body(OctreeNode *node, Body *body);
void calculate_force(OctreeNode *node, Body *body, double force[]);
void update_body(Body bodies[], int num_bodies, OctreeNode *root);
void free_octree(OctreeNode *node);


void write_position(Body *body);

#endif
