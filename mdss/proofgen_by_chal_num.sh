#!/bin/bash
for num in {100..1000..100}
do
echo $num >> ./logs/proofgen_by_chal_num2.log
for index in {1..10..1}
do
str="${num}.out"
./$str >> ./logs/proofgen_by_chal_num2.log
make clear-audit -i
done
done