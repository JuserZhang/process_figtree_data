process_figtree_data: process_figtree_data.o
	gcc process_figtree_data.c -o process_figtree_data
 
process_figtree_data.o:process_figtree_data.c
	gcc process_figtree_data.c -c -Wall -g -o process_figtree_data.o
clean:
	rm -rf ./process_figtree_data process_figtree_data.o