export MOOSE_JOBS=1
export METHOD=opt

export CFLAGS='-O2 -march=native' 
export CXXFLAGS='-O2 -march=native' 
export FCFLAGS='-O2 -march=native'
export FFLAGS='-O2 -march=native'

cd $MOOSE_DIR/scripts
./update_and_rebuild_petsc.sh COPTFLAGS=$CFLAGS CXXOPTFLAGS=$CXXFLAGS FCOPTFLAGS=$FCFLAGS FOPTFLAGS=$FFLAGS --with-mpi-dir=/opt/mpich-4.2.1
./update_and_rebuild_libmesh.sh --with-mpi-dir=/opt/mpich-4.2.1
./update_and_rebuild_wasp.sh

cd $MOOSE_DIR/test
make -j $MOOSE_JOBS
