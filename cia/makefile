link_files = main.c ./utils/algos.c ./utils/randys.c ./utils/sha256.c -lgmp
outputs = exec
DO = data_owner
csps = "0" "1" "2" "3" "4" "5" "6" "7"
all: store sec_prepare sec_submit chal_gen proof_gen proof_submit proof_verify
store: 
	gcc $(DO)/$(link_files) -DSTORE -o $(DO)/$(outputs)/store.out
sec_prepare: 
	for id in $(csps); do gcc csp$$id/$(link_files) -DSEC_PREPARE -DID=\"$$id\" -o csp$$id/$(outputs)/sec_prepare.out; done
sec_submit:
	for id in $(csps); do gcc csp$$id/$(link_files) -DSEC_SUBMIT -DID=\"$$id\" -o csp$$id/$(outputs)/sec_submit.out; done
chal_gen:
	for id in $(csps); do gcc csp$$id/$(link_files) -DCHAL_GEN -DID=\"$$id\" -o csp$$id/$(outputs)/chal_gen.out; done
proof_gen:
	for id in $(csps); do gcc csp$$id/$(link_files) -DPROOF_GEN -DID=\"$$id\" -o csp$$id/$(outputs)/proof_gen.out; done
proof_submit:
	for id in $(csps); do gcc csp$$id/$(link_files) -DPROOF_SUBMIT -DID=\"$$id\" -o csp$$id/$(outputs)/proof_submit.out; done
proof_verify:
	for id in $(csps); do gcc csp$$id/$(link_files) -DPROOF_VERIFY -DID=\"$$id\" -o csp$$id/$(outputs)/proof_verify.out; done
clear-audit:
	rm ./storage/swap/*
	for id in $(csps); do rm ./storage/csp$$id/secrets/*; done
clear-exec:
	for id in $(csps); do rm csp$$id/$(outputs)/*; done
	rm $(DO)/$(outputs)/*
clear-all: clear-audit clear-exec
	rm ./storage/*.info
	for id in $(csps); do find ./storage/csp$$id/blocks/ -type f | xargs rm -f ; done