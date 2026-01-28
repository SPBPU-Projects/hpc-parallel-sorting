#!/bin/bash
#SBATCH --job-name=pthreads_sort
#SBATCH --output=slurm-pthreads-%j.out
#SBATCH --error=slurm-pthreads-%j.err
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=8

make pthreads
./bin/sort_pthreads 8 data/input_500.txt
