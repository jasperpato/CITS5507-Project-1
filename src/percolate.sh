#
# Script for overwriting N and N_THREADS in constant.h before executing program.
#
# USAGE
#  ./percolate.sh  [-s | -b]  N  P  [N_THREADS]
# OR
#  ./percolate.sh  [-s | -b]  -f  FILENAME  N  [N_THREADS]
#
# NOTE: to use this script you must specify [-s | -b]

if [ $1 == "-s"  ] || [ $1 == "-b" ]; then
  make -B util/params
  if [[ $2 == "-f" ]]; then
    if [[ $5 ]]; then
      ./util/params $4 $5
    else
      ./util/params $4 1
    fi
    make -B percolate
    ./percolate $1 -f $3
  else
    if [[ $4 ]]; then
      ./util/params $2 $4 
    else
      ./util/params $2 1
    fi
    make -B percolate
    ./percolate $1 $3
  fi
fi
