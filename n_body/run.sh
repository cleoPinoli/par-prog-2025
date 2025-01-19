# Command use to compile all the .c files to return a nbody executable file.
mpicc -o nbody main.c nbody_serial.c nbody_parallel.c nbody_utils.c -lm;
chmod +x nbody;

# Serial exection with 1.000 bodies in 1.000 steps.
./nbody 500 500;

# Parallel exection using 4 processes.
# mpiexec -n 4 ./nbody 1000 1000;
