cd src;

# Check if an argument is provided.
if [ -z "$1" ]; then
    echo "Use -> ./run.sh <version>"
    exit 1
fi

VERSION=$1


# Command use to compile all the .c files to return a nbody executable file.
mpicc -o nbody ./$VERSION/main.c nbody_serial.c nbody_parallel.c nbody_octtree.c nbody_utils.c -lm;
chmod +x nbody;

# Serial exection with 1.000 bodies in 1.000 steps.
# ./nbody 100 1000;

# Serial exection with 1.000 bodies in 1.000 steps with the octtree implementation.
# ./nbody 100 1000 --octtree;

# Parallel exection using 4 processes.
# mpiexec -n 4 ./nbody 100 100;
