# OpenMPI Allgather VS. Customer All-to-all

## Description

This project aims to analyze and compare the performance of the OpenMPI `Allgather` collective communication operation with a custom implementation of the `All-to-All` communication pattern. The `Allgather` operation is commonly used in parallel computing to gather data from all processes in a communicator and distribute the combined data back to all processes.

The objective is to evaluate the efficiency, scalability, and overhead of both approaches in various scenarios, such as different message sizes, number of processes, and communication patterns. By conducting extensive experiments on MSstate's Supercomputer "Shadow," we aim to gain insights into the strengths and weaknesses of each method and provide recommendations for choosing the appropriate approach based on specific requirements and constraints.

## Setup and Execution (WILL NOT WORK! FOR POSTERITY!)

1. Connect to MSstate's Supercomputer "Shadow" using your preferred SSH client:

   ```bash
   ssh -Y shadow-login
   ```



2. Load the required modules for compiling and executing MPI programs:

    ```bash
    module load openmpi slurm 
    ```
3. Copy the project files from your local machine to the "Shadow" cluster and change to the project directory:

    ```bash
    scp -r project-directory shadow-login:~ /project-directory && cd project-directory
    ```

4. Compile the project:

    ```bash
    mpicc -o executable_name source_file.c
    ```
5. Submit the job using Slurm:

    ```bash
    sbatch job_script.js
    ```
6. Monitor the status of your job:
    ```bash
    squeue -u username
    ```
Once the job is completed, you will find the output in the specified file.
