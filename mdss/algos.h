#ifndef ALGOS_H
#define ALGOS_H
#include "./utils/merkletree.h"
#include <pbc/pbc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern void psd_permute(int *array, int n, unsigned int seed, int N);
extern void sha256(const unsigned char *data, size_t len, unsigned char *out);
extern int pairing_init(pairing_t pairing, const char *pairing_param_file_name);
extern void write_str_to_file(unsigned char *_str, size_t _size,
                              const char *file_name);
extern void read_str_from_file(unsigned char *_str, size_t _size,
                               const char *file_name);
void read_int_from_file(int *num, size_t _size, const char *file_name);
void write_int_to_file(int *num, size_t _size, const char *file_name);                               
extern void H1(element_t g1, char *str, int str_len);
extern void H2(element_t zr, element_t g1, int g1_size);
extern void H3(element_t zr_op, element_t zr_ip, char *str, int str_len);
extern void H4(element_t g1, char *str, int str_len);

int read_elem_from_file(element_t gg, char type, char *base, char *name);
int write_elem_to_file(element_t gg, char type, char *base, char *name);
int init_vars(void);
int key_gen(void);
int copy_gen(void);
int tag_gen(void);
int store(void);
int chal_gen(void);
int proof_gen(void);
int proof_verify(void);
int clear_vars(void);

#endif