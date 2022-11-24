.SILENT:
test:
	gcc main.c matrix_gen.c ./utils/utils.c algos.c -lgmp -DTEST -o main.out
main:
	gcc main.c matrix_gen.c ./utils/utils.c algos.c -lgmp -UTEST -o main.out
clear:
	cd storage && bash clear.sh
ce:
	cd /media/randy/f546d18d-01da-4906-9fd9-6de41f280504/storage && bash clear.sh