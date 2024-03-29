#!/bin/bash
#SBATCH --account=193000-cf0001
#SBATCH --job-name=Work04P
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=4
#SBATCH --time=00:20:00
#SBATCH --no-reque
#SBATCH --qos=debug
module load openmpi
mpirun -np 4 ./project3 >& results.04p


