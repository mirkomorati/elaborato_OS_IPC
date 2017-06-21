#!/bin/bash

lower=$1
upper=$2
P=$3

for i in `seq $lower $upper`; do
    ../elaborato_IPC -A ../../sample/m_in/matrixA$i -B ../../sample/m_in/matrixB$i -C ../../sample/m_out/matrixC$i -N $i -P $P
done
