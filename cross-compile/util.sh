#!/bin/bash


measure_func_time () {
    start=$(date +%s.%N)
    ($1) 
    duration=$(echo "$(date +%s.%N) - $start" | bc)
    execution_time=`printf "%.2f seconds" $duration`
    printf "${BGreen}$1 Execution Time: $execution_time${Color_Off}\n"
}
