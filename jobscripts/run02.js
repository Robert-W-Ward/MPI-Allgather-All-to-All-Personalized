#!/bin/bash
#SBATCH --account=193000-cf0001
#SBATCH --job-name=Work02P
#SBATCH --nodes=1
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:20:00
#SBATCH --no-reque
#SBATCH --qos=debug
module load openmpi
mpirun -np 2 ./project3 >& results.02p


