#!/bin/bash

for t in control naive lut_4bit lut_8bit hacker hacker2 wiki3; do
    echo $t
    for n in $(seq 1 5); do
        time -p ./bitcount-test $t
        echo
    done
done
