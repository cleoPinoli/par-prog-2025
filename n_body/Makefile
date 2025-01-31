CC = mpicc
CFLAGS = -O2 -lm

all: serial parallel

serial: src/main.c src/nbody_serial.c src/nbody_utils.c
	$(CC) $(CFLAGS) -o nbody_serial src/main.c src/nbody_serial.c src/nbody_utils.c

parallel: src/main.c src/nbody_parallel.c src/nbody_utils.c
	$(CC) $(CFLAGS) -o nbody_parallel src/main.c src/nbody_parallel.c src/nbody_utils.c

clean:
	rm -f nbody_serial nbody_parallel
