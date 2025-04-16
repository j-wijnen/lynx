module load foss/2023a

# Set compiler flags
export METHODS=opt
export CC=mpicc CXX=mpicxx FC=mpif90 F90=mpif90 F77=mpif77
export CFLAGS='-O2 -march=native -ftree-vectorize'
export CXXFLAGS='-O2 -march=native -ftree-vectorize'
export FCFLAGS='-O2 -march=native -ftree-vectorize'
export FFLAGS='-O2 -march=native -ftree-vectorize'

cd $DATA/moose/scripts
./update_and_rebuild_petsc.sh CC=mpicc CXX=mpicxx FC=mpif90 F77=mpif77 F90=mpif90 COPTFLAGS='-O2 -march=native -ftree-vectorize' \
  CXXOPTFLAGS='-O2 -march=native -ftree-vectorize' FCOPTFLAGS='-O2 -march=native -ftree-vectorize' \
  FOPTFLAGS='-O2 -march=native -ftree-vectorize' --download-cmake || return
./update_and_rebuild_libmesh.sh || return
./update_and_rebuild_wasp.sh || return

cd $DATA/moose/test
make -j $MOOSE_JOBS

cd $DATA/lynx/
make -j $MOOSE_JOBS
