#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Error: Missing arguments."
    echo "Usage: $0 <i_start> <i_end> <j_start> <j_end>"
    echo "Example: $0 5 10 3 8  (Loops i from 5-10, j from 3-8)"
    exit 1
fi

cd bin

echo "Looping from sc(2^$1 to 2^$2), so(2^$3 to 2^$4)"
echo "" > data.csv

first=true


for ((k=5; k<=6; k++)); do
    board=$((2**k))
    for ((i=$1; i<=$2; i++)); do
        
        sc=$((2**i))
        
        echo "i=$i, sc=2^$i = $sc"
        
        for ((j=$3; j<=$4; j++)); do
            oc=$((2**j))
            
            echo "  j=$j, oc=2^$j = $oc"
            
            ./navegation -w 1920 -h 1080 -gw $board -gh $board -sc $sc -oc $oc -ra 32 -sa 1

            if [ "$first" = true ]; then
                cat log.csv >> ./data.csv
                first=false
            else
                tail -n +2 log.csv  >> ./data.csv
            fi
        done
    done
done

echo "Data collection complete."
echo "Starting python plotting"

python ./graph.py