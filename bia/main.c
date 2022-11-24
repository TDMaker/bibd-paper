#include "algos.h"

int main(void)
{
    printf("Settings:\nFile size: %lu Byte (%lu KB, %lu MB). \n%lu CSPs(Components).\n", (COMP_SIZE * (unsigned long)(v)), COMP_SIZE * (unsigned long)(v) / 1024, COMP_SIZE * (unsigned long)(v) / 1024 / 1024, (unsigned long)(v));
    printf("Component size: %d Byte (%d KB, %d MB), Block size: %d Byte\n", COMP_SIZE, COMP_SIZE / 1024, COMP_SIZE / 1024 / 1024, BLK_SIZE);
    printf("There are %d blocks in a component, challenging %d of them.\n\n", COMP_SIZE / BLK_SIZE, CHAL_AMOUNT);
    bibd_init();
    // store();
    // damage("/media/randy/f546d18d-01da-4906-9fd9-6de41f280504/storage/k3/cs5/8", 400, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    // damage("/media/randy/f546d18d-01da-4906-9fd9-6de41f280504/storage/k3/cs5/11", 500, "YYYYYYYYYYYYYYYYYYYYYYYYYYY");
    // damage("storage/k3/cs4/7", 500, "YYYYYYYYYYYYYYYYYYYYYYYYYYY");
    // pproof_gen(challenge());
    // cproof_gen();
    // verify();
    clear();
    return 0;
}