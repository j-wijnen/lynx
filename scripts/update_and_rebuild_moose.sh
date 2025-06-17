if [ -z ${MOOSE_JOBS} ]
then 
  export MOOSE_JOBS=1
fi

echo "Number of cpus " $MOOSE_JOBS

export CC=mpicc
export CXX=mpicxx
export FC=mpif90
export F90=mpif90
export F77=mpif77

export METHODS=opt
export METHOD=opt

FLAGS="-O2 -march=native -funroll-loops"
export CFLAGS=$FLAGS
export CXXFLAGS=$FLAGS
export FCFLAGS=$FLAGS
export FFLAGS=$FLAGS

cd $MOOSE_DIR/test
git pull --recurse-submodules

cd $MOOSE_DIR/scripts

./update_and_rebuild_petsc.sh CC=$CC CXX=$CXX FC=$FC \
  COPTFLAGS="$CFLAGS" CXXOPTFLAGS="$CXXFLAGS" FOPTFLAGS="$FFLAGS"
./update_and_rebuild_libmesh.sh
./update_and_rebuild_wasp.sh

cd $MOOSE_DIR/test
make -j $MOOSE_JOBS
