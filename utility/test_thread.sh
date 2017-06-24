#!/bin/bash

lower=$1
upper=$2
matlab_cmd=""

for i in `seq $lower $upper`; do
    echo -e "\n======================"
    echo "ELABORATO ORDER $i"
    echo
    ../elaborato_IPC -A ../../sample/m_in/matrixA$i -B ../../sample/m_in/matrixB$i -C ../../sample/m_out/matrixC$i -N $i -t > /dev/null
    matlab_cmd="$matlab_cmd A=csvread('../../sample/m_in/matrixA$i');B=csvread('../../sample/m_in/matrixB$i');csvwrite('../../sample/m_out/matlab_matrixC$i', A*B);"
done

matlab_cmd="$matlab_cmd exit;"

echo -e "\n======================"
echo "MATLAB WORKING"
/Applications/MATLAB_R2017a.app/bin/matlab -nodesktop -nosplash -nojvm -sd ../../sample/m_in -r "$matlab_cmd" > /dev/null
echo

for i in `seq $lower $upper`; do
    echo -e "\n======================"
    echo "DIFF ORDER $i"
    ./matrix_diff ../../sample/m_out/matrixC$i ../../sample/m_out/matlab_matrixC$i $i > /dev/null
    echo $?
    echo
done
