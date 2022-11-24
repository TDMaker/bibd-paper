#include "algos.h"
#include "settings.h"
mt_t *mt;
mt_hash_t root;
clock_t start, finish;
pairing_t pairing;
int *m_i;
int g1_size, zr_size;

int init_vars(void)
{
    memset(root, 0, sizeof(mt_hash_t));
    mt = mt_create();
    m_i = (int *)malloc(TOTAL_NUMBER * sizeof(int));
    for (int i = 0; i < TOTAL_NUMBER; i++) {
        m_i[i] = VAR_M;
    }
    if (pairing_init(pairing, PAIRING_PARAM_FILE_PATH) < 0)
        exit(0);

    g1_size = pairing_length_in_bytes_G1(pairing);
    zr_size = pairing_length_in_bytes_Zr(pairing);

    return 0;
}
int key_gen(void)
{
    element_t g, mpk, sk_O1, sk_O2, pk_O, msk, g1_tmp, zr_tmp;
    element_init_G1(g, pairing);
    element_init_G1(mpk, pairing);

    element_init_G1(sk_O1, pairing);
    element_init_Zr(sk_O2, pairing);
    element_init_G1(pk_O, pairing);

    element_init_Zr(msk, pairing);
    element_init_G1(g1_tmp, pairing);
    element_init_Zr(zr_tmp, pairing);

    element_random(msk);
    write_elem_to_file(msk, 'z', "", VAR_MSK);
    element_random(g);
    write_elem_to_file(g, 'g', "", VAR_G);
    element_pow_zn(mpk, g, msk);
    write_elem_to_file(mpk, 'g', "", VAR_MPK);

    H1(g1_tmp, ID_O, strlen(ID_O));
    element_pow_zn(sk_O1, g1_tmp, msk);
    write_elem_to_file(sk_O1, 'g', "", VAR_SK_O1);

    H2(zr_tmp, sk_O1, g1_size);
    element_random(sk_O2);
    write_elem_to_file(sk_O2, 'z', "", VAR_X_O);
    element_add(sk_O2, sk_O2, zr_tmp);
    write_elem_to_file(sk_O2, 'z', "", VAR_SK_O2);

    element_pow_zn(pk_O, g, sk_O2);
    write_elem_to_file(pk_O, 'g', "", VAR_PK_O);

    element_clear(msk);
    element_clear(g);
    element_clear(mpk);
    element_clear(sk_O1);
    element_clear(sk_O2);
    element_clear(pk_O);
    element_clear(g1_tmp);
    element_clear(zr_tmp);

    return 0;
}
int copy_gen(void)
{
    FILE *fp;
    if ((fp = fopen(FILE_PATH, "r")) == NULL) {
        printf("Fail to open the sourced file!\n");
        exit(0);
    }
    unsigned char *blk_buf = (unsigned char *)malloc(BLK_SIZE * sizeof(unsigned char));
    char blk_sig_path[64];
    char str_buf[32] = {0};
    element_t b_i, b_ij, sk_O2;
    element_init_Zr(b_i, pairing);
    element_init_Zr(b_ij, pairing);
    element_init_Zr(sk_O2, pairing);
    read_elem_from_file(sk_O2, 'z', "", VAR_SK_O2);
    sprintf(str_buf, "%s%s", VAR_NAME, "||");
    const int offset = strlen(str_buf);
    for (int i = 0; i < TOTAL_NUMBER; i++) {
        fread(blk_buf, BLK_SIZE, 1, fp);
        element_from_hash(b_i, blk_buf, BLK_SIZE);
        for (int j = 0; j < m_i[i]; j++) {
            sprintf(str_buf + offset, "%d%c%c%c%c", j, 0, 0, 0, 0);
            H3(b_ij, sk_O2, str_buf, strlen(str_buf));
            element_add(b_ij, b_i, b_ij);
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%d_%d", i, j);
            write_elem_to_file(b_ij, 'z', BLK_BASE, blk_sig_path);
        }
    }
    element_clear(b_i);
    element_clear(b_ij);
    element_clear(sk_O2);
    free(blk_buf);
    fclose(fp);
    return 0;
}
int tag_gen(void)
{
    char blk_sig_path[64] = {0};
    element_t b_ij, sigma_i, sigma_ij, g1_tmp, a, g, A, sk_O1, sk_O2;
    unsigned char *zr_str = pbc_malloc(zr_size);
    unsigned char(*inner_hash_val_aggrd)[32];
    unsigned char inner_hash_value[32] = {0};
    char bid_ij[128] = {0};
    unsigned char(*h_i)[32] = (unsigned char(*)[32])malloc(TOTAL_NUMBER * 32 * sizeof(unsigned char));

    memset(h_i, 0, TOTAL_NUMBER * 32 * sizeof(unsigned char));
    element_init_Zr(b_ij, pairing);
    element_init_G1(sigma_i, pairing);
    element_init_G1(sigma_ij, pairing);
    element_init_G1(g1_tmp, pairing);
    element_init_Zr(a, pairing);
    element_init_G1(g, pairing);
    element_init_G1(A, pairing);
    element_init_G1(sk_O1, pairing);
    element_init_Zr(sk_O2, pairing);

    read_elem_from_file(g, 'g', "", VAR_G);
    element_random(a);
    element_pow_zn(A, g, a);
    write_elem_to_file(A, 'g', "", VAR_A);
    read_elem_from_file(g, 'g', "", VAR_G);
    read_elem_from_file(sk_O1, 'g', "", VAR_SK_O1);
    read_elem_from_file(sk_O2, 'z', "", VAR_SK_O2);
    sprintf(bid_ij, "%s%s%s%s", VAR_NAME, "||", ID_O, "||");
    const int offset = strlen(bid_ij);
    int offset2;
    start = clock();
    for (int i = 0; i < TOTAL_NUMBER; i++) {
        inner_hash_val_aggrd = (unsigned char(*)[32])malloc(m_i[i] * 32 * sizeof(unsigned char));
        for (int j = 0; j < m_i[i]; j++) {
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%d_%d", i, j);
            read_elem_from_file(b_ij, 'z', BLK_BASE, blk_sig_path);
            element_to_bytes(zr_str, b_ij);
            sha256(zr_str, zr_size, inner_hash_value);
            memcpy(inner_hash_val_aggrd[j], inner_hash_value, 32);
        }

        sha256((unsigned char *)inner_hash_val_aggrd, m_i[i] * 32, h_i[i]);
        mt_add(mt, h_i[i], HASH_LENGTH);

        element_set1(sigma_i);
        for (int j = 0; j < m_i[i]; j++) {
            sprintf(bid_ij + offset, "%d%s%d%s%c%c%c%c", m_i[i], "||", j, "||", 0, 0, 0, 0);
            offset2 = strlen(bid_ij);
            memcpy(bid_ij + offset2, h_i[i], 32);
            H4(g1_tmp, bid_ij, offset2 + 32);
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%d_%d", i, j);
            read_elem_from_file(b_ij, 'z', BLK_BASE, blk_sig_path);
            element_mul(b_ij, a, b_ij);
            element_pow_zn(sigma_ij, g, b_ij);
            element_mul(sigma_ij, g1_tmp, sigma_ij);
            element_pow_zn(sigma_ij, sigma_ij, sk_O2);
            element_mul(sigma_ij, sk_O1, sigma_ij);
            element_mul(sigma_i, sigma_i, sigma_ij);
        }
        free(inner_hash_val_aggrd);
        memset(blk_sig_path, 0, 64);
        sprintf(blk_sig_path, "%d", i);
        write_elem_to_file(sigma_i, 'g', SIG_BASE, blk_sig_path);
    }
    finish = clock();
    write_str_to_file((unsigned char *)h_i, TOTAL_NUMBER * 32 * sizeof(unsigned char), VAR_H_I);
    printf("Tag generation costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    element_clear(b_ij);
    element_clear(sigma_i);
    element_clear(sigma_ij);
    element_clear(g1_tmp);
    element_clear(a);
    element_clear(g);
    element_clear(A);
    element_clear(sk_O1);
    element_clear(sk_O2);
    pbc_free(zr_str);
    free(h_i);
    return 0;
}
int store(void)
{
    return 0;
}
int chal_gen(void)
{
    size_t seed;
    unsigned char *zr_str = pbc_malloc(zr_size);
    read_str_from_file((unsigned char *)&seed, sizeof(size_t), "/dev/urandom");
    int *u = (int *)malloc(CHAL_NUMBER * sizeof(int));
    psd_permute(u, CHAL_NUMBER, seed, TOTAL_NUMBER);
    write_int_to_file(u, CHAL_NUMBER * sizeof(int), VAR_U);

    FILE *fp;
    if ((fp = fopen(VAR_R_I, "w")) == NULL) {
        printf("Fail to open file *%s*\n", VAR_R_I);
        exit(0);
    }
    element_t r_i;
    element_init_Zr(r_i, pairing);
    pbc_random_set_deterministic(seed);
    for (int i = 0; i < CHAL_NUMBER; i++) {
        element_random(r_i);
        element_to_bytes(zr_str, r_i);
        fwrite(zr_str, 1, zr_size, fp);
    }
    fclose(fp);
    free(u);
    pbc_free(zr_str);
    return 0;
}
int proof_gen(void)
{
    FILE *fp;
    if ((fp = fopen(VAR_R_I, "r")) == NULL) {
        printf("Fail to open file *%s*\n", VAR_R_I);
        exit(0);
    }
    element_t *r_i = (element_t *)malloc(CHAL_NUMBER * sizeof(element_t));
    unsigned char *zr_str = pbc_malloc(zr_size);
    for (int i = 0; i < CHAL_NUMBER; i++) {
        fread(zr_str, zr_size, 1, fp);
        element_init_Zr(r_i[i], pairing);
        element_from_bytes(r_i[i], zr_str);
    }
    fclose(fp);

    int *u = (int *)malloc(CHAL_NUMBER * sizeof(int));
    read_int_from_file(u, CHAL_NUMBER * sizeof(int), VAR_U);

    char blk_sig_path[64];
    element_t sigma_i, b_ij, b_ij_sum, sigma, mu;
    element_init_G1(sigma_i, pairing);
    element_init_Zr(b_ij, pairing);
    element_init_Zr(b_ij_sum, pairing);
    element_init_G1(sigma, pairing);
    element_init_Zr(mu, pairing);
    element_set1(sigma);
    element_set0(mu);
    start = clock();
    for (int i = 0; i < CHAL_NUMBER; i++) {
        memset(blk_sig_path, 0, 64);
        sprintf(blk_sig_path, "%d", u[i]);
        read_elem_from_file(sigma_i, 'g', SIG_BASE, blk_sig_path);
        element_pow_zn(sigma_i, sigma_i, r_i[i]);
        element_mul(sigma, sigma, sigma_i);
        element_set0(b_ij_sum);
        for (int j = 0; j < m_i[u[i]]; j++) {
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%d_%d", u[i], j);
            read_elem_from_file(b_ij, 'z', BLK_BASE, blk_sig_path);
            element_add(b_ij_sum, b_ij_sum, b_ij);
        }
        element_mul(b_ij_sum, b_ij_sum, r_i[i]); // TODO: issue of _zn
        element_add(mu, mu, b_ij_sum);
    }
    finish = clock();
    printf("Proof generation costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    write_elem_to_file(sigma, 'g', "", VAR_SIGMA);
    write_elem_to_file(mu, 'z', "", VAR_MU);
    element_clear(sigma_i);
    element_clear(b_ij);
    element_clear(b_ij_sum);
    element_clear(sigma);
    element_clear(mu);
    free(u);
    for (int i = 0; i < CHAL_NUMBER; i++) {
        element_clear(r_i[i]);
    }
    free(r_i);
    pbc_free(zr_str);
    return 0;
}
int proof_verify(void)
{
    FILE *fp;
    if ((fp = fopen(VAR_R_I, "r")) == NULL) {
        printf("Fail to open file *%s*\n", VAR_R_I);
        exit(0);
    }
    element_t *r_i = (element_t *)malloc(CHAL_NUMBER * sizeof(element_t));
    unsigned char *zr_str = pbc_malloc(zr_size);
    for (int i = 0; i < CHAL_NUMBER; i++) {
        fread(zr_str, zr_size, 1, fp);
        element_init_Zr(r_i[i], pairing);
        element_from_bytes(r_i[i], zr_str);
    }
    fclose(fp);

    element_t g1_tmp, zr_tmp, ri_sum, gt_right, gt_left, inner_prod, outter_prod, A, sigma, mu, pk_O, mpk, g;
    element_init_G1(g1_tmp, pairing);
    element_init_Zr(zr_tmp, pairing);
    element_init_G1(inner_prod, pairing);
    element_init_G1(outter_prod, pairing);
    element_init_Zr(ri_sum, pairing);
    element_init_GT(gt_right, pairing);
    element_init_GT(gt_left, pairing);
    element_init_G1(A, pairing);
    element_init_G1(sigma, pairing);
    element_init_Zr(mu, pairing);
    element_init_G1(pk_O, pairing);
    element_init_G1(mpk, pairing);
    element_init_G1(g, pairing);
    read_elem_from_file(A, 'g', "", VAR_A);
    read_elem_from_file(sigma, 'g', "", VAR_SIGMA);
    read_elem_from_file(mu, 'z', "", VAR_MU);
    read_elem_from_file(pk_O, 'g', "", VAR_PK_O);
    read_elem_from_file(mpk, 'g', "", VAR_MPK);
    read_elem_from_file(g, 'g', "", VAR_G);
    int *u = (int *)malloc(CHAL_NUMBER * sizeof(int));
    read_int_from_file(u, CHAL_NUMBER * sizeof(int), VAR_U);
    unsigned char(*h_i)[32] = (unsigned char(*)[32])malloc(TOTAL_NUMBER * 32 * sizeof(unsigned char));
    read_str_from_file((unsigned char *)h_i, TOTAL_NUMBER * 32 * sizeof(unsigned char), VAR_H_I);
    char bid_ij[128] = {0};
    sprintf(bid_ij, "%s%s%s%s", VAR_NAME, "||", ID_O, "||");
    const int offset = strlen(bid_ij);
    int offset2;
    element_set1(outter_prod);
    element_set0(ri_sum);
    element_pow_zn(A, A, mu);
    for (int i = 0; i < CHAL_NUMBER; i++) {
        element_mul_si(zr_tmp, r_i[i], m_i[u[i]]);
        element_add(ri_sum, ri_sum, zr_tmp);
        element_set1(inner_prod);
        for (int j = 0; j < m_i[u[i]]; j++) {
            sprintf(bid_ij + offset, "%d%s%d%s%c%c%c%c", m_i[u[i]], "||", j, "||", 0, 0, 0, 0);
            offset2 = strlen(bid_ij);
            memcpy(bid_ij + offset2, h_i[u[i]], 32);
            H4(g1_tmp, bid_ij, offset2 + 32);
            element_pow_zn(g1_tmp, g1_tmp, r_i[i]);
            element_mul(inner_prod, inner_prod, g1_tmp);
        }
        element_mul(outter_prod, outter_prod, inner_prod);
    }
    element_mul(outter_prod, outter_prod, A);
    pairing_apply(gt_right, outter_prod, pk_O, pairing);

    H1(g1_tmp, ID_O, strlen(ID_O));
    element_pow_zn(g1_tmp, g1_tmp, ri_sum);
    pairing_apply(gt_left, g1_tmp, mpk, pairing);

    element_mul(gt_right, gt_left, gt_right);

    pairing_apply(gt_left, sigma, g, pairing);

    if (!element_cmp(gt_left, gt_right)) {
        printf("proof verifies\n");
    } else {
        printf("proof does not verify\n");
    }
    finish = clock();
    printf("Proof verification costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);

    element_clear(g1_tmp);
    element_clear(inner_prod);
    element_clear(outter_prod);
    element_clear(ri_sum);
    element_clear(gt_left);
    element_clear(gt_right);

    element_clear(A);
    element_clear(sigma);
    element_clear(mu);
    element_clear(pk_O);
    element_clear(mpk);
    element_clear(g);
    free(h_i);
    free(u);
    for (int i = 0; i < CHAL_NUMBER; i++) {
        element_clear(r_i[i]);
    }
    free(r_i);
    pbc_free(zr_str);
    return 0;
}
int clear_vars(void)
{
    free(m_i);
    pairing_clear(pairing);
    return 0;
}
int write_elem_to_file(element_t gg, char type, char *base, char *name)
{
    char path_str[64] = {0};
    sprintf(path_str, "%s%s", base, name);
    int elem_size = type == 'g' ? g1_size : zr_size;
    unsigned char *elem_str = pbc_malloc(elem_size);
    element_to_bytes(elem_str, gg);
    write_str_to_file(elem_str, elem_size, path_str);
    pbc_free(elem_str);
    return 0;
}

int read_elem_from_file(element_t gg, char type, char *base, char *name)
{
    char path_str[64] = {0};
    sprintf(path_str, "%s%s", base, name);
    int elem_size = type == 'g' ? g1_size : zr_size;
    unsigned char *elem_str = pbc_malloc(elem_size);
    read_str_from_file(elem_str, elem_size, path_str);
    element_from_bytes(gg, elem_str);
    pbc_free(elem_str);
    return 0;
}
