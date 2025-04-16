export METHOD=opt

if [[ $CFLAGS != *"-march=native -ftree-vectorize"* ]]; then
  export CFLAGS=$CFLAGS' -march=native -ftree-vectorize' 
fi
if [[ $CXXFLAGS != *"-march=native -ftree-vectorize"* ]]; then
  export CXXFLAGS=$CXXFLAGS' -march=native -ftree-vectorize' 
fi
if [[ $FCFLAGS != *"-march=native -ftree-vectorize"* ]]; then
  export FCFLAGS=$FCFLAGS' -march=native -ftree-vectorize' 
fi
if [[ $FFLAGS != *"-march=native -ftree-vectorize"* ]]; then
  export FFLAGS=$FFLAGS' -march=native -ftree-vectorize' 
fi
