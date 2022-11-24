#ifndef ALGOS_H
#define ALGOS_H
#include "utils/settings.h"
#include "utils/utils.h"
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
extern int generate_es(int E[][k + 1]);
extern int generate_from(int E[][k + 1], int F[][v]);
extern int generate_d(int E[][k + 1], int D[][k + 1]);
int bibd_init(void);
int store(void);
size_t challenge(void);
int pproof_gen(size_t);
int cproof_gen(void);
int verify(void);
int damage(const char *filename, long pos, const char *value);
int clear(void);

#endif /* ALGOS_H*/