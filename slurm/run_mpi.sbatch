#!/bin/bash
#SBATCH --job-name=mpi_sort
#SBATCH --output=slurm-mpi-%j.out
#SBATCH --error=slurm-mpi-%j.err
#SBATCH --time=00:10:00
#SBATCH --ntasks=4

module load openmpi || true

make mpi
mpirun -np 4 ./bin/sort_mpi data/input_500.txt
