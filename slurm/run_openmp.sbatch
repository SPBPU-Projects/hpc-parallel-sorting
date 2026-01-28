#!/bin/bash
#SBATCH --job-name=openmp_sort
#SBATCH --output=slurm-openmp-%j.out
#SBATCH --error=slurm-openmp-%j.err
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=8

export OMP_NUM_THREADS=8

make openmp
./bin/sort_openmp data/input_500.txt
