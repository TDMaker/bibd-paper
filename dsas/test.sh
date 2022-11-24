#!/bin/bash
for index in {1..10..1}
do
#make clean -i
#make main
echo ${index}======================== >> ./logs/proof_by_replica.log
./a.out >> ./logs/proof_by_replica.log
done