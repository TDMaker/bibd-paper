#ifndef RANDYS_H
#define RANDYS_H

#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
extern void sha256(const unsigned char *data, size_t len, unsigned char *out);
void write_str_to_file(unsigned char *_str, size_t _size,
                       const char *file_name);
void read_str_from_file(unsigned char *_str, size_t _size,
                        const char *file_name);
void psd_permute(size_t *array, size_t n, unsigned int seed, size_t N);
void psd_func(size_t *array, size_t n, unsigned int seed, size_t q);
#endif /* randys_h */