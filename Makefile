CC=gcc
MPICC=mpicc
CFLAGS=-O3 -Wall
OMPFLAGS=-fopenmp
PTHREADFLAGS=-pthread

BIN_DIR=bin

MPI_SRC=src/sort_mpi.c
OMP_SRC=src/sort_openmp.c
PTHREAD_SRC=src/sort_pthreads.c

MPI_BIN=$(BIN_DIR)/sort_mpi
OMP_BIN=$(BIN_DIR)/sort_openmp
PTHREAD_BIN=$(BIN_DIR)/sort_pthreads

all: prep mpi openmp pthreads

prep:
	@mkdir -p $(BIN_DIR)

mpi:
	$(MPICC) $(CFLAGS) $(MPI_SRC) -o $(MPI_BIN)

openmp:
	$(CC) $(CFLAGS) $(OMPFLAGS) $(OMP_SRC) -o $(OMP_BIN)

pthreads:
	$(CC) $(CFLAGS) $(PTHREADFLAGS) $(PTHREAD_SRC) -o $(PTHREAD_BIN)

clean:
	rm -rf $(BIN_DIR) *.o
