#!/bin/bash
#SBATCH --account=193000-cf0001
#SBATCH --job-name=Debug12P
#SBATCH --nodes=1
#SBATCH --ntasks=12
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=12
#SBATCH --time=00:02:00
#SBATCH --no-reque
#SBATCH --qos=debug
module load openmpi
mpirun -np 12 ./project3 1 >& out.12p


