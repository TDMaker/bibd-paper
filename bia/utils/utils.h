#ifndef UTILS_H 
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_str_to_file(unsigned char *_str, size_t _size, char *file_name,
                       FILE *fp);
void read_str_from_file(unsigned char *_str, size_t _size, char *file_name,
                        FILE *fp);
void psd_permute(size_t *array, size_t n, unsigned int seed, size_t N);
void psd_func(size_t *array, size_t n, unsigned int seed, size_t q);

int init(void *addr, char type, const char *value, int length);
int find_max_index(int *addr, int length);
int find_max(int *addr, int length);
int is_in(int value, int *addr, int length);
int remove_ele(int value, int *addr, int length);
#endif /* UTILS_H */