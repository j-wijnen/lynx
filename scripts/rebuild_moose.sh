if [ -z ${MOOSE_JOBS} ]
then 
  export MOOSE_JOBS=1
fi

if [ -z ${METHODS} ]
then 
  export METHODS=opt
fi

if [ -z ${MPI_DIR} ]
then
  LINK_MPI_DIR=''
else
  LINK_MPI_DIR=--with-mpi-dir=$MPI_DIR
fi

# Set compiler flags
export CFLAGS='-O2 -march=native -ftree-vectorize'
export CXXFLAGS='-O2 -march=native -ftree-vectorize'
export FCFLAGS='-O2 -march=native -ftree-vectorize'
export FFLAGS='-O2 -march=native -ftree-vectorize'

cd $MOOSE_DIR/scripts
./update_and_rebuild_petsc.sh COPTFLAGS='-O2 -march=native -ftree-vectorize' CXXOPTFLAGS='-O2 -march=native -ftree-vectorize' \
  FCOPTFLAGS='-O2 -march=native -ftree-vectorize' FOPTFLAGS='-O2 -march=native -ftree-vectorize' $LINK_MPI_DIR || return
./update_and_rebuild_libmesh.sh $LINK_MPI_DIR || return
./update_and_rebuild_wasp.sh || return

cd $MOOSE_DIR/test
make -j $MOOSE_JOBS
