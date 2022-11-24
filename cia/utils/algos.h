#ifndef ID
#define ID "2333"
#endif
#include "./randys.h"
#include "./settings.h"

extern void sha256(const unsigned char *data, size_t len, unsigned char *out);
int store(void);
int sec_prepare(void);
int sec_submit(void);
int chal_gen(void);
int proof_gen(void);
int proof_submit(void);
int proof_verify(void);
