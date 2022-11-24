#include "algos.h"

int main(void)
{
    init_vars();
#ifndef AUDIT
    key_gen();
    copy_gen();
    tag_gen();
    store();
#endif
    chal_gen();
    proof_gen();
    proof_verify();
    clear_vars();
    return 0;
}