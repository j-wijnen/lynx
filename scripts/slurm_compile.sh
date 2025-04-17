# To run interactive on arc
# srun --nodes=1 --tasks-per-node=12 --mem=24gb --time=04:00:00 -p interactive --pty /bin/bash

module load foss/2022b
module load CMake/3.24.3-GCCcore-12.2.0
module load HDF5/1.14.0-gompi-2022b
module load UCX/1.14.0-GCCcore-12.2.0

# Set compiler flags
export METHODS=opt
export CC=mpicc CXX=mpicxx FC=mpif90 F90=mpif90 F77=mpif77
export CFLAGS='-O2 -march=native -ftree-vectorize'
export CXXFLAGS='-O2 -march=native -ftree-vectorize'
export FCFLAGS='-O2 -march=native -ftree-vectorize'
export FFLAGS='-O2 -march=native -ftree-vectorize'

cd $HOME/moose/scripts
./update_and_rebuild_petsc.sh CC=mpicc CXX=mpicxx FC=mpif90 F77=mpif77 F90=mpif90 COPTFLAGS='-O2 -march=native -ftree-vectorize' \
  CXXOPTFLAGS='-O2 -march=native -ftree-vectorize' FCOPTFLAGS='-O2 -march=native -ftree-vectorize' \
  FOPTFLAGS='-O2 -march=native -ftree-vectorize' || return
./update_and_rebuild_libmesh.sh || return
./update_and_rebuild_wasp.sh|| return

cd $HOME/moose/test
make -j $MOOSE_JOBS

cd $HOME/lynx/
make -j $MOOSE_JOBS
