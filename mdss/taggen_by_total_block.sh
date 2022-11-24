#!/bin/bash
for num in {1000..6000..1000}
do
echo $num >> ./logs/taggen_by_total_block.log
for index in {1..10..1}
do
str="${num}.out"
echo =======================${index} >> ./logs/taggen_by_total_block.log
./$str >> ./logs/taggen_by_total_block.log
done
done