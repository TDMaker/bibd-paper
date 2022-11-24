#!/bin/bash
log_file=./logs/proof_by_replica.log
num=8
echo $num >> ${log_file}
for index in {1..10..1}
do
str="${num}.out"
echo =======================${index} >> ${log_file}
./$str >> ${log_file}
done