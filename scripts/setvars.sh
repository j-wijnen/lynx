export METHOD=opt

export CC=mpicc 
export CXX=mpicxx 
export FC=mpif90

FLAGS="-O2 -march=native -funroll-loops -ftree-vectorize"
if [[ $CFLAGS != *"$FLAGS"* ]]; then
  export CFLAGS=$CFLAGS" $FLAGS" 
fi
if [[ $CXXFLAGS != *"$FLAGS"* ]]; then
  export CXXFLAGS=$CXXFLAGS" $FLAGS" 
fi
if [[ $FFLAGS != *"$FLAGS"* ]]; then
  export FFLAGS=$FFLAGS" $FLAGS" 
fi
#if [[ $FFLAGS != *"$FLAGS"* ]]; then
#  export FFLAGS=$FFLAGS" $FLAGS" 
#fi
