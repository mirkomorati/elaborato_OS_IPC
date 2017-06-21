#!/bin/bash

path=$1
lower=$2
upper=$3

# CIAO

for i in `seq $lower $upper`; do
    ./matrix_gen $path/matrixA$i $i
    ./matrix_gen $path/matrixB$i $i
done
