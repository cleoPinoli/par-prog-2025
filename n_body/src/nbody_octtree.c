#include "nbody_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


OctreeNode *create_octree_node(double center[], double half_size) {
    OctreeNode *node = (OctreeNode*)malloc(sizeof(OctreeNode));

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


void insert_body(OctreeNode *node, Body *body) {

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
            insert_body(node, existing_body);
        }

        int index = 0;
        if (body->x > node->center[0]) index |= 1 << 0;
        if (body->y > node->center[1]) index |= 1 << 1;
        if (body->z > node->center[2]) index |= 1 << 2;

        if (node->children[index] == NULL) {
            double child_center[DIM];
            double offset = node->half_size / 2;

            for (int i = 0; i < DIM; i++) {
                child_center[i] = node->center[i] + ((index & (1 << i)) ? offset : -offset);
            }

            // printf("%d %d %d\n", child_center[0], child_center[1], child_center[2]);
            node->children[index] = create_octree_node(child_center, offset);
        }

        insert_body(node->children[index], body);

        double total_mass = node->mass + body->mass;
        node->center_of_mass[0] = (node->center_of_mass[0] * node->mass + body->x * body->mass) / total_mass;
        node->center_of_mass[1] = (node->center_of_mass[1] * node->mass + body->y * body->mass) / total_mass;
        node->center_of_mass[2] = (node->center_of_mass[2] * node->mass + body->z * body->mass) / total_mass;
        // printf("%d %d %d\n", node->center_of_mass[0], node->center_of_mass[1], node->center_of_mass[0]);

        node->mass = total_mass;
    }
}


void calculate_force(OctreeNode *node, Body *body, double force[]) {

    if (!node || node->mass == 0.0) {
        return;
    }

    double dx = node->center_of_mass[0] - body->x;
    double dy = node->center_of_mass[1] - body->y;
    double dz = node->center_of_mass[2] - body->z;
    double r = sqrt(dx * dx + dy * dy + dz * dz);

    if (r == 0) {
        return;
    }

    if ((node->half_size / r) < THETA || node->body) {
        double gravitational_force = (G * node->mass * body->mass) / (r * r + EPS);

        force[0] += gravitational_force * dx / r;
        force[1] += gravitational_force * dy / r;
        force[2] += gravitational_force * dz / r;

    } else {
        for (int i = 0; i < 8; i++) {
            calculate_force(node->children[i], body, force);
        }
    }
}


void update_body(Body bodies[], int num_bodies, OctreeNode *root) {
    for (int i = 0; i < num_bodies; i++) {
        double force[DIM] = {0.0};

        calculate_force(root, &bodies[i], force);

        bodies[i].vx += (force[0] / bodies[i].mass) * DT;
        bodies[i].x += bodies[i].vx * DT;

        bodies[i].vy += (force[1] / bodies[i].mass) * DT;
        bodies[i].y += bodies[i].vy * DT;

        bodies[i].vz += (force[2] / bodies[i].mass) * DT;
        bodies[i].z += bodies[i].vz * DT;
    }
}


void free_octree(OctreeNode *node) {
    if (!node){
        return;
    }

    for (int i = 0; i < 8; i++) {
        free_octree(node->children[i]);
    }

    free(node);
}


void simulate_octtree(Body *bodies, int n, int steps) {
    FILE *file = fopen("../positions.txt", "a");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    double root_half_size = BOUND / 2;
    double root_center[DIM] = {root_half_size, root_half_size, root_half_size}; // Center of the cube.

    OctreeNode *root = create_octree_node(root_center, root_half_size);

    for (int i = 0; i < n; i++) {
        insert_body(root, &bodies[i]);
    }

   for (int step = 0; step < steps; step++) {
        update_body(bodies, n, root);

        for (int i = 0; i < n; i++) {
            fprintf(file, "%.4f, %.4f, %.4f\n", bodies[i].x, bodies[i].y, bodies[i].z);
            fflush(file);
        }
    }

    free_octree(root);
    fclose(file);
}
