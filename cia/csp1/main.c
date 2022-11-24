#include "../utils/algos.h"

int main(void)
{
#ifdef SEC_PREPARE
    sec_prepare();
#endif
#ifdef SEC_SUBMIT
    sec_submit();
#endif
#ifdef CHAL_GEN
    chal_gen();
#endif
#ifdef PROOF_GEN
    proof_gen();
#endif
#ifdef PROOF_SUBMIT
    proof_submit();
#endif
#ifdef PROOF_VERIFY
    proof_verify();
#endif
    return 0;
}