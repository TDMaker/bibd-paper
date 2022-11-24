#include "./algos.h"
int store(void)
{
    printf("Into Storing...\n");
    clock_t start, finish;
    size_t block_size = DEFAULT_BLK_SIZE;
    FILE *fp;
    char str_buffer[128] = {0};
    char dest_csp[CSP_AMOUNT][32];
    int masked_name = rand();
    if ((fp = fopen(FILE_PATH, "r")) == NULL) {
        printf("Fail to open file *%s*\n", FILE_PATH);
        exit(0);
    }
    size_t block_real_size = 0;
    unsigned char *file_block = malloc(block_size * sizeof(unsigned char));
    size_t index = 0;
    start = clock();
    while ((block_real_size = fread(file_block, 1, block_size, fp)) > 0 && index < BLOCK_AMOUNT) {
        for (int i = 0; i < CSP_AMOUNT; i++) {
            memset(dest_csp[i], 0, 32);
            sprintf(dest_csp[i], "%s/csp%d/blocks/%d_blk_%ld", BASE_PATH, i, masked_name, index);
            write_str_to_file(file_block, block_real_size, dest_csp[i]);
        }
        memset(file_block, 0, block_size);
        index++;
    }
    finish = clock();
    fclose(fp);

    memset(str_buffer, 0, 128);
    sprintf(str_buffer, "%s/%s.info", BASE_PATH, SOURCED_FILE_NAME);
    if ((fp = fopen(str_buffer, "w")) == NULL) {
        printf("Fail to open file *%s*\n", str_buffer);
        exit(0);
    }
    fprintf(fp, "%ld\n%d", index, masked_name);
    fclose(fp);

    free(file_block);
    printf("Store done.\n");
    printf("Blocks store costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    return 0;
}
int sec_prepare(void)
{
    printf("Into SEC_PREPARE of CSP%s\n", ID);
    FILE *fp;
    char file_path_buf[128] = {0};
    unsigned char hash_value[32] = {0};
    mpz_t r;
    mpz_init(r);
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    size_t seed;
    read_str_from_file((unsigned char *)&seed, sizeof(size_t), "/dev/urandom");

    gmp_randseed_ui(state, seed);
    mpz_urandomb(r, state, SECURITY_LEVEL);
    char *r_str = mpz_get_str(0, 10, r);
    sha256((unsigned char *)r_str, strlen(r_str), hash_value);
    sprintf(file_path_buf, "%s/%s.sec.hv%s", FILE_PUB_PATH, SOURCED_FILE_NAME, ID);
    write_str_to_file(hash_value, 32, file_path_buf);

    sprintf(file_path_buf, "%s/%s.sec.hk%s", FILE_SEC_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_out_str(fp, 10, r);
    mpz_clear(r);
    free(r_str);
    fclose(fp);
    printf("SEC_PREPARE of CSP%s done.\n", ID);
    return 0;
}

int sec_submit(void)
{
    printf("Into SEC_SUBMIT of CSP%s\n", ID);
    FILE *fp;
    mpz_t r;
    mpz_init(r);
    char file_path_buf[128] = {0};
    sprintf(file_path_buf, "%s/%s.sec.hk%s", FILE_SEC_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_inp_str(r, fp, 10);
    fclose(fp);

    sprintf(file_path_buf, "%s/%s.sec.hk%s", FILE_PUB_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_out_str(fp, 10, r);
    mpz_clear(r);
    fclose(fp);
    printf("SEC_SUBMIT of CSP%s done.\n", ID);
    return 0;
}

int chal_gen(void)
{
    printf("Into CHAL_GEN of CSP%s\n", ID);
    FILE *fp;
    mpz_t r, sum_r;
    mpz_init(r);
    mpz_init(sum_r);
    char file_path_buf[128] = {0};
    unsigned char hash_value_from_file[32] = {0};
    unsigned char hash_value_computed[32] = {0};
    char *r_str;
    for (int i = 0; i < CSP_AMOUNT; i++) {
        memset(file_path_buf, 0, 128);
        sprintf(file_path_buf, "%s/%s.sec.hk%d", FILE_PUB_PATH, SOURCED_FILE_NAME, i);
        if ((fp = fopen(file_path_buf, "r")) == NULL) {
            printf("Fail to open file *%s*\n", file_path_buf);
            exit(0);
        }
        mpz_inp_str(r, fp, 10);
        fclose(fp);
        r_str = mpz_get_str(0, 10, r);
        sha256((unsigned char *)r_str, strlen(r_str), hash_value_computed);
        free(r_str);
        memset(file_path_buf, 0, 128);
        sprintf(file_path_buf, "%s/%s.sec.hv%d", FILE_PUB_PATH, SOURCED_FILE_NAME, i);
        read_str_from_file(hash_value_from_file, 32, file_path_buf);
        if (memcmp(hash_value_from_file, hash_value_computed, 32)) {
            printf("CSP%s reports CSP%d NOT pass the Sec 2PC\n", ID, i);
            continue;
        }
        mpz_add(sum_r, sum_r, r);
    }
    memset(file_path_buf, 0, 128);
    sprintf(file_path_buf, "%s/%s.sec.aggred%s", FILE_SEC_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_out_str(fp, 10, sum_r);
    fclose(fp);
    mpz_clear(r);
    mpz_clear(sum_r);
    printf("CHAL_GEN of CSP%s done.\n", ID);
    return 0;
}

int proof_gen(void)
{
    printf("Into PROOF_GEN of CSP%s\n", ID);
    clock_t start, finish;
    FILE *fp;
    char buffer[64] = {0};
    char file_path_buf[128] = {0};
    unsigned char hash_value[32] = {0};
    size_t block_size = DEFAULT_BLK_SIZE;
    size_t blk_amount = 0;
    mpz_t b_i, nu_j, sum_r, my_r, P;
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    mpz_init(b_i);
    mpz_init(nu_j);
    mpz_init(sum_r);
    mpz_init(my_r);
    mpz_init(P);

    memset(file_path_buf, 0, 128);
    sprintf(file_path_buf, "%s/%s.sec.aggred%s", FILE_SEC_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_inp_str(sum_r, fp, 10);
    fclose(fp);
    size_t seed = mpz_get_ui(sum_r);
    gmp_randseed(state, sum_r);

    memset(file_path_buf, 0, 128);
    sprintf(file_path_buf, "%s/%s.info", BASE_PATH, SOURCED_FILE_NAME);
    if ((fp = fopen(file_path_buf, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    fgets(buffer, 64, fp);
    blk_amount = atoi(buffer);
    memset(buffer, 0, 64);
    fgets(buffer, 64, fp);
    fclose(fp);

    size_t *mu_j = (size_t *)malloc(sizeof(size_t) * CHAL_AMOUNT);
    psd_permute(mu_j, CHAL_AMOUNT, seed, blk_amount);

    unsigned char *blk_buf = (unsigned char *)malloc(block_size * sizeof(unsigned char));
    start = clock();
    for (size_t j = 0; j < CHAL_AMOUNT; j++) {
        memset(file_path_buf, 0, 128);
        sprintf(file_path_buf, "%s/csp%s/blocks/%s_blk_%ld", BASE_PATH, ID, buffer, mu_j[j]);
        memset(blk_buf, 0, block_size * sizeof(unsigned char));
        read_str_from_file(blk_buf, block_size, file_path_buf);
        mpz_import(b_i, block_size, 1, sizeof(unsigned char), 0, 0, blk_buf);
        mpz_urandomb(nu_j, state, SECURITY_LEVEL);
        mpz_mul(b_i, b_i, nu_j);
        mpz_mod(b_i, b_i, sum_r);
        mpz_add(P, P, b_i);
    }
    finish = clock();
    mpz_mod(P, P, sum_r);

    sprintf(file_path_buf, "%s/%s.proof.hk%s", FILE_SEC_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_out_str(fp, 10, P);
    fclose(fp);

    sprintf(file_path_buf, "%s/%s.sec.hk%s", FILE_PUB_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_inp_str(my_r, fp, 10);
    mpz_add(my_r, my_r, P);
    char *r_str = mpz_get_str(0, 10, my_r);
    sha256((unsigned char *)r_str, strlen(r_str), hash_value);
    free(r_str);
    sprintf(file_path_buf, "%s/%s.proof.hv%s", FILE_PUB_PATH, SOURCED_FILE_NAME, ID);
    write_str_to_file(hash_value, 32, file_path_buf);

    free(mu_j);
    free(blk_buf);
    mpz_clear(b_i);
    mpz_clear(nu_j);
    mpz_clear(sum_r);
    mpz_clear(my_r);
    mpz_clear(P);
    puts("==========================");
    printf("%f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    printf("PROOF_GEN of CSP%s done.\n", ID);
    puts("==========================");
    return 0;
}

int proof_submit(void)
{
    printf("Into PROOF_SUBMIT of CSP%s\n", ID);
    FILE *fp;
    mpz_t P;
    mpz_init(P);
    char file_path_buf[128] = {0};
    sprintf(file_path_buf, "%s/%s.proof.hk%s", FILE_SEC_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_inp_str(P, fp, 10);
    fclose(fp);

    sprintf(file_path_buf, "%s/%s.proof.hk%s", FILE_PUB_PATH, SOURCED_FILE_NAME, ID);
    if ((fp = fopen(file_path_buf, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_path_buf);
        exit(0);
    }
    mpz_out_str(fp, 10, P);
    mpz_clear(P);
    fclose(fp);
    printf("PROOF_SUBIT of CSP%s done.\n", ID);
    return 0;
}

int proof_verify(void)
{
    printf("Into PROOF_VERIFY of CSP%s\n", ID);
    FILE *fp;
    unsigned char buffer[128] = {0};
    char file_path_buf[128] = {0};
    mpz_t P, r;
    unsigned char hash_value_from_file[32] = {0};
    unsigned char hash_value_computed[32] = {0};
    size_t P_truncated = 0;
    mpz_init(P);
    mpz_init(r);

    char *r_str;
    for (int i = 0; i < CSP_AMOUNT; i++) {
        memset(file_path_buf, 0, 128);
        sprintf(file_path_buf, "%s/%s.proof.hk%d", FILE_PUB_PATH, SOURCED_FILE_NAME, i);
        if ((fp = fopen(file_path_buf, "r")) == NULL) {
            printf("Fail to open file *%s*\n", file_path_buf);
            exit(0);
        }
        mpz_inp_str(P, fp, 10);
        fclose(fp);
        memset(file_path_buf, 0, 128);
        sprintf(file_path_buf, "%s/%s.sec.hk%d", FILE_PUB_PATH, SOURCED_FILE_NAME, i);
        if ((fp = fopen(file_path_buf, "r")) == NULL) {
            printf("Fail to open file *%s*\n", file_path_buf);
            exit(0);
        }
        mpz_inp_str(r, fp, 10);
        fclose(fp);
        mpz_add(r, r, P);

        r_str = mpz_get_str(0, 10, r);
        sha256((unsigned char *)r_str, strlen(r_str), hash_value_computed);
        free(r_str);
        memset(file_path_buf, 0, 128);
        sprintf(file_path_buf, "%s/%s.proof.hv%d", FILE_PUB_PATH, SOURCED_FILE_NAME, i);
        read_str_from_file(hash_value_from_file, 32, file_path_buf);
        if (memcmp(hash_value_from_file, hash_value_computed, 32)) {
            printf("CSP%s reports CSP%d NOT pass the PROOF 2PC\n", ID, i);
        }
        printf("P from CSP%d is %zu\n", i, mpz_get_ui(P));
    }
    mpz_clear(P);
    mpz_clear(r);
    printf("PROOF_VERIFY of CSP%s done.\n", ID);
    return 0;
}