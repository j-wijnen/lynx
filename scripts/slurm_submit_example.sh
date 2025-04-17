#!/bin/bash
#SBATCH --job-name=mm
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --tasks-per-node=8
#SBATCH --mem=24gb
#SBATCH --time=12:00:00

# Copy job data to $TMPDIR
cd $TMPDIR || exit 1
rsync -az $SLURM_SUBMIT_DIR/ ./

# Load modules
module load foss/2022b
module load CMake/3.24.3-GCCcore-12.2.0
module load HDF5/1.14.0-gompi-2022b
module load UCX/1.14.0-GCCcore-12.2.0

# Execute
mpiexec -n $SLURM_NTASKS $HOME/lynx/lynx-opt -i inputfile.i

# Copy data back from $TMPDIR to $DATA/myproject directory
rm slurm-*.out
rsync -az ./ $SLURM_SUBMIT_DIR/