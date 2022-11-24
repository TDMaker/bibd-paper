#ifndef ALGOS_H
#define ALGOS_H
#include "settings.h"
#include <gmp.h>
#include <pbc/pbc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern void write_str_to_file(unsigned char *_str, size_t _size,
                              const char *file_name);
extern void read_str_from_file(unsigned char *_str, size_t _size,
                               const char *file_name);
extern void H1(element_t result, element_t g, mpz_t pz_tmp, char *source);
extern void psd_permute(int *array, int nn, size_t seed, size_t N);
extern int pairing_init(pairing_t pairing, const char *pairing_param_file_name);
extern int get_sha256(const char *filename, unsigned char rdresult[]);
int write_g1_to_file(element_t gg, char *base, char *name);
int read_g1_from_file(element_t gg, char *base, char *name);
int get_tau(unsigned char (*tau)[32]);
int get_component_name(char **component, int index);

int init_vars(void);
int setup(void);
int key_gen(void);
int file_split(void);
int sig_gen(void);
int chall_gen(void);
int proof_gen(void);
int aggregation(void);
int self_verify(void);
int clear_vars(void);

#endif