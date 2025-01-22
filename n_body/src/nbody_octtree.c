#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


#define THETA 0.5
#define G 1.0
#define TIMESTEP 0.01
#define DIM 3

typedef struct {
    double x, y, z;
    double vx, vy, vz;
    double mass;
} Body;

typedef struct OctreeNode {
    double center[DIM];
    double half_size;
    double mass;
    double center_of_mass[DIM];
    Body *body;
    struct OctreeNode *children[8];
} OctreeNode;

OctreeNode *create_octree_node(double center[], double half_size);
void insert_particle(OctreeNode *node, Body *body);
void calculate_force(OctreeNode *node, Body *body, double force[]);
void update_particles(Body bodies[], int num_bodies, OctreeNode *root);
void free_octree(OctreeNode *node);

OctreeNode *create_octree_node(double center[], double half_size) {
    OctreeNode *node = (OctreeNode *)malloc(sizeof(OctreeNode));
    for (int i = 0; i < DIM; i++) {
        node->center[i] = center[i];
        node->center_of_mass[i] = 0.0;
    }
    node->half_size = half_size;
    node->mass = 0.0;
    node->body = NULL;
    for (int i = 0; i < 8; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void insert_particle(OctreeNode *node, Body *body) {
    if (node->body == NULL && node->mass == 0.0) {
        node->body = body;
        node->mass = body->mass;
        node->center_of_mass[0] = body->x;
        node->center_of_mass[1] = body->y;
        node->center_of_mass[2] = body->z;
    } else {
        if (node->body) {
            Body *existing_body = node->body;
            node->body = NULL;
            insert_particle(node, existing_body);
        }

        int index = 0;
        if (body->x > node->center[0]) index |= 1 << 0;
        if (body->y > node->center[1]) index |= 1 << 1;
        if (body->z > node->center[2]) index |= 1 << 2;

        if (node->children[index] == NULL) {
            double child_center[DIM];
            double offset = node->half_size / 2.0;
            for (int i = 0; i < DIM; i++) {
                child_center[i] = node->center[i] + ((index & (1 << i)) ? offset : -offset);
            }
            node->children[index] = create_octree_node(child_center, offset);
        }

        insert_particle(node->children[index], body);

        double total_mass = node->mass + body->mass;
        node->center_of_mass[0] = (node->center_of_mass[0] * node->mass + body->x * body->mass) / total_mass;
        node->center_of_mass[1] = (node->center_of_mass[1] * node->mass + body->y * body->mass) / total_mass;
        node->center_of_mass[2] = (node->center_of_mass[2] * node->mass + body->z * body->mass) / total_mass;
        node->mass = total_mass;
    }
}

void calculate_force(OctreeNode *node, Body *body, double force[]) {
    if (!node || node->mass == 0.0) return;

    double dx = node->center_of_mass[0] - body->x;
    double dy = node->center_of_mass[1] - body->y;
    double dz = node->center_of_mass[2] - body->z;
    double distance = sqrt(dx*dx + dy*dy + dz*dz);

    if (distance == 0.0) return;

    if ((node->half_size / distance) < THETA || node->body) {
        double magnitude = (G * node->mass * body->mass) / (distance * distance + 1e-9);
        force[0] += magnitude * dx / distance;
        force[1] += magnitude * dy / distance;
        force[2] += magnitude * dz / distance;
    } else {
        for (int i = 0; i < 8; i++) {
            calculate_force(node->children[i], body, force);
        }
    }
}

void update_particles(Body bodies[], int num_bodies, OctreeNode *root) {
    for (int i = 0; i < num_bodies; i++) {
        double force[DIM] = {0.0};
        calculate_force(root, &bodies[i], force);

        bodies[i].vx += (force[0] / bodies[i].mass) * TIMESTEP;
        bodies[i].x += bodies[i].vx * TIMESTEP;

        bodies[i].vy += (force[1] / bodies[i].mass) * TIMESTEP;
        bodies[i].y += bodies[i].vy * TIMESTEP;

        bodies[i].vz += (force[2] / bodies[i].mass) * TIMESTEP;
        bodies[i].z += bodies[i].vz * TIMESTEP;
    }
}

void free_octree(OctreeNode *node) {
    if (!node) return;
    for (int i = 0; i < 8; i++) {
        free_octree(node->children[i]);
    }
    free(node);
}


int main() {
    
    FILE *file = fopen("../positions.txt", "a");

    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    double start_time, end_time;

    int num_bodies = 100;
    int steps = 1000;

    Body *bodies = (Body*)malloc(num_bodies * sizeof(Body));

    for (int i = 0; i < num_bodies; i++) {
        bodies[i].x = (double)(rand() % 1000);
        bodies[i].y = (double)(rand() % 1000);
        bodies[i].z = (double)(rand() % 1000);
        bodies[i].vx = 0.0;
        bodies[i].vy = 0.0;
        bodies[i].vz = 0.0;
        bodies[i].mass = 1e14 + ((float)rand() / RAND_MAX) * (1e20 - 1e16);
    }

    // Adjust root node to cover particle positions (0-1000 in all dimensions)
    double root_center[DIM] = {500.0, 500.0, 500.0}; // Center of the 0-1000 cube
    double root_half_size = 500.0; // Half-size to span 0-num_bodies

    OctreeNode *root = create_octree_node(root_center, root_half_size);

    for (int i = 0; i < num_bodies; i++) {
        insert_particle(root, &bodies[i]);
    }


    start_time = MPI_Wtime();

    for (int t = 0; t < steps; t++) {
        update_particles(bodies, num_bodies, root);

        for (int i = 0; i < num_bodies; i++) {
            fprintf(file, "%.4f, %.4f, %.4f\n", bodies[i].x, bodies[i].y, bodies[i].z);
            fflush(file);
        }
    }

    end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    printf("Execution time: %.4f seconds.\n", elapsed_time);

    fclose(file);

    free(bodies);
    free_octree(root);
    return 0;
}
