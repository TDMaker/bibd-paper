#!/bin/bash
for num in {200..1000..100}
do
echo $num >> proof_gen_by_chal_amount.log
for index in {1..10..1}
do
str="${num}.out"
./$str >> proof_gen_by_chal_amount.log
done
done