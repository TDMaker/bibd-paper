.SILENT:
main:
	gcc main.c ./utils/randys.c ./utils/sha256.c algos.c -lpbc -lgmp -Lutils -l:libMerkleTree.a -o a.out
audit:
	gcc main.c ./utils/randys.c ./utils/sha256.c algos.c -lpbc -lgmp -Lutils -l:libMerkleTree.a -DAUDIT -o a.out
clear-audit:
	rm ./storage/r_i
	rm ./storage/sigma
	rm ./storage/u
	rm ./storage/mu
clear-all: clear-audit
	find ./storage/blks/ -type f | xargs rm -f
	find ./storage/sigs/ -type f | xargs rm -f
	rm ./storage/A
	rm ./storage/g
	rm ./storage/h_i
	rm ./storage/mpk
	rm ./storage/pk_O
	rm ./storage/secrets/msk
	rm ./storage/secrets/sk_O1
	rm ./storage/secrets/sk_O2
	rm ./storage/secrets/x_O
