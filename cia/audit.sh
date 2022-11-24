#!/bin/bash
csps=(csp0 csp1 csp2 csp3 csp4 csp5 csp6 csp7)
for csp in ${csps[@]}; do $csp/exec/sec_prepare.out; done
for csp in ${csps[@]}; do $csp/exec/sec_submit.out; done
for csp in ${csps[@]}; do $csp/exec/chal_gen.out; done
for csp in ${csps[@]}; do $csp/exec/proof_gen.out; done
for csp in ${csps[@]}; do $csp/exec/proof_submit.out; done
for csp in ${csps[@]}; do $csp/exec/proof_verify.out; done
