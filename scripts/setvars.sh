export METHOD=opt

export CC=mpicc
export CXX=mpicxx
export FC=mpif90
export F90=mpif90
export F77=mpif77

FLAGS="-O2 -march=native -funroll-loops"
if [[ $CFLAGS != *"$FLAGS"* ]]; then
 export CFLAGS=$CFLAGS" $FLAGS" 
fi
if [[ $CXXFLAGS != *"$FLAGS"* ]]; then
 export CXXFLAGS=$CXXFLAGS" $FLAGS" 
fi
if [[ $FFLAGS != *"$FLAGS"* ]]; then
 export FFLAGS=$FFLAGS" $FLAGS" 
fi
if [[ $FCFLAGS != *"$FCLAGS"* ]]; then
 export FCFLAGS=$FCFLAGS" $FLAGS" 
fi
