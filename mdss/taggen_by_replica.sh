#!/bin/bash
for num in {2..8..1}
do
echo $num >> taggen_by_replica.log
for index in {1..10..1}
do
str="${num}.out"
./$str >> taggen_by_replica.log
make clear-all -i
done
done