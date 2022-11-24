.SILENT:
main:
	gcc main.c ./utils/randys.c ./utils/sha256_file.c ./algos.c -lssl -lcrypto -lgmp -lpbc -o a.out
audit:
	gcc main.c ./utils/randys.c ./utils/sha256_file.c ./algos.c -lssl -lcrypto -lgmp -lpbc -DAUDIT -o a.out
clean:
	find ./storage/blks/ -type f | xargs rm -f
	find ./storage/sigs/ -type f | xargs rm -f
	rm ./storage/params/*