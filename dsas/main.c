#include "algos.h"
int main(void)
{
    init_vars();
#ifndef AUDIT
    setup();
    key_gen();
    file_split();
    sig_gen();
#endif
    chall_gen();
    proof_gen();
    aggregation();
    self_verify();
    clear_vars();
    return 0;
}