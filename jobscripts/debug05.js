#!/bin/bash
#SBATCH --account=193000-cf0001
#SBATCH --job-name=Debug05P
#SBATCH --nodes=1
#SBATCH --ntasks=5
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=5
#SBATCH --time=00:02:00
#SBATCH --no-reque
#SBATCH --qos=debug
module load openmpi
mpirun -np 5 ./project3 1 >& out.05p


