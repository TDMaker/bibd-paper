#include "algos.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief
 * Case file c
 * each component is 28672 Byte length
 * namely, 1792 blocks devided into
 *
 * Case file d
 * each component is 1909248 Byte length
 * namely, 119,328 blocks divided into
 * @return int
 */

clock_t start, finish;
pairing_t pairing;
element_t g, g1, alpha, y, z;
element_t sigma_ij, g1_tmp;
mpz_t b_ij, pz_tmp;
int g1_size;
unsigned char *g1_str;
int *J;
element_t *u, *sigma, *mu, *R;

element_t P, Lambda;

int init_vars(void)
{
    if (pairing_init(pairing, PAIRING_PARAM_FILE_PATH) < 0)
        exit(0);
    element_init_G1(g, pairing);
    element_init_G1(g1, pairing);
    element_init_G1(y, pairing);
    element_init_G1(z, pairing);
    element_init_Zr(alpha, pairing);

    g1_size = pairing_length_in_bytes_compressed_G1(pairing);
    g1_str = pbc_malloc(g1_size);

    mpz_init(b_ij);
    mpz_init(pz_tmp);
    element_init_G1(sigma_ij, pairing);
    element_init_G1(g1_tmp, pairing);

    return 0;
}

int setup(void)
{
    element_random(g);
    element_random(g1);

    write_g1_to_file(g, PRA_BASE, "g");
    write_g1_to_file(g1, PRA_BASE, "g1");

    return 0;
}

int key_gen(void)
{
    read_g1_from_file(g, PRA_BASE, "g");
    read_g1_from_file(g1, PRA_BASE, "g1");

    element_random(alpha);

    element_pow_zn(y, g, alpha);
    write_g1_to_file(y, PRA_BASE, "y");

    element_pow_zn(z, g1, alpha);
    write_g1_to_file(z, PRA_BASE, "z");

    return 0;
}

int file_split(void)
{
    printf("Into FILE_SPLIT...\n");
    unsigned char blk_buf[BLK_SIZE];
    char path_str[64];
    char blk_sig_path[64];
    FILE *fp;
    char *component;
    for (int i = 0; i < VAR_M; i++) {
        memset(path_str, 0, 64);
        get_component_name(&component, i);
        sprintf(path_str, "%s%s", COM_BASE, component);
        free(component);
        if ((fp = fopen(path_str, "r")) == NULL) {
            printf("Fail to open file *%s*\n", path_str);
            exit(0);
        }
        for (int j = 0; j < VAR_N; j++) {
            fread(blk_buf, BLK_SIZE, 1, fp);
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%s%d_%d", BLK_BASE, i, j);
            write_str_to_file(blk_buf, BLK_SIZE, blk_sig_path);
        }
        fclose(fp);
    }
    printf("FILE_SPLIT done.\n\n");
    return 0;
}

int sig_gen(void)
{
    printf("Into SIG_GEN...\n");
    unsigned char blk_buf[BLK_SIZE];
    char path_str[64];
    char hash_key[64];
    char blk_sig_path[64];
    int offset;
    unsigned char(*tau)[32] = malloc(VAR_M * 32 * sizeof(unsigned char));
    read_g1_from_file(g, PRA_BASE, "g");
    get_tau(tau);
    start = clock();
    for (int i = 0; i < VAR_M; i++) {
        memset(path_str, 0, 64);
        memset(hash_key, 0, 64);
        memcpy(hash_key, tau[i], 32);
        sprintf(hash_key + 32, "%s%d%s", "||", i, "||");
        offset = strlen(hash_key);
        for (int j = 0; j < VAR_N; j++) {
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%s%d_%d", BLK_BASE, i, j);
            read_str_from_file(blk_buf, BLK_SIZE, blk_sig_path);
            mpz_import(b_ij, BLK_SIZE, 1, sizeof(unsigned char), 0, 0, blk_buf);
            sprintf(hash_key + offset, "%d%c%c%c", j, '\0', '\0', '\0');
            H1(g1_tmp, g, pz_tmp, hash_key);
            element_pow_mpz(sigma_ij, g, b_ij);
            element_mul(sigma_ij, sigma_ij, g1_tmp);
            element_pow_zn(sigma_ij, sigma_ij, alpha);

            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%s%d_%d", SIG_BASE, i, j);
            write_g1_to_file(sigma_ij, blk_sig_path, "");
        }
    }
    finish = clock();
    printf("SigGen costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    free(tau);
    return 0;
}

int chall_gen(void)
{
    size_t seed;
    read_str_from_file((unsigned char *)&seed, sizeof(size_t), "/dev/urandom");
    J = (int *)malloc(VAR_S * sizeof(int));
    psd_permute(J, VAR_S, seed, VAR_N);
    u = (element_t *)malloc(VAR_S * sizeof(element_t));
    for (int j = 0; j < VAR_S; j++) {
        element_init_Zr(u[j], pairing);
        element_random(u[j]);
        // element_printf("%B\n", u[j]);
    }
    return 0;
}

int proof_gen(void)
{
    element_t expon, prod;
    element_init_G1(expon, pairing);
    element_init_Zr(prod, pairing);

    mpz_t r_i;
    mpz_init(r_i);
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, (unsigned)time(0));

    sigma = (element_t *)malloc(VAR_M * sizeof(element_t));
    mu = (element_t *)malloc(VAR_M * sizeof(element_t));
    R = (element_t *)malloc(VAR_M * sizeof(element_t));

    unsigned char blk_buf[BLK_SIZE];
    char path_str[64];
    char hash_key[64];
    char blk_sig_path[64];

    start = clock();
    for (int i = 0; i < VAR_M; i++) {
        element_init_G1(sigma[i], pairing);
        element_set1(sigma[i]);
        element_init_Zr(mu[i], pairing);
        element_set0(mu[i]);
        element_init_G1(R[i], pairing);
        for (int j = 0; j < VAR_S; j++) {
            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%s%d_%d", BLK_BASE, i, J[j]);
            read_str_from_file(blk_buf, BLK_SIZE, blk_sig_path);
            mpz_import(b_ij, BLK_SIZE, 1, sizeof(unsigned char), 0, 0, blk_buf);

            memset(blk_sig_path, 0, 64);
            sprintf(blk_sig_path, "%s%d_%d", SIG_BASE, i, J[j]);
            read_g1_from_file(sigma_ij, blk_sig_path, "");

            element_pow_zn(expon, sigma_ij, u[j]);
            element_mul(sigma[i], sigma[i], expon);

            element_mul_mpz(prod, u[j], b_ij);
            element_add(mu[i], mu[i], prod);
        }
        mpz_urandomb(r_i, state, SECURITY_LEVEL);
        element_pow_mpz(R[i], y, r_i);

        element_to_bytes_compressed(g1_str, R[i]);
        mpz_import(pz_tmp, g1_size, 1, sizeof(unsigned char), 0, 0, g1_str);
        mpz_mul(r_i, r_i, pz_tmp);
        element_set_mpz(prod, r_i);
        element_add(mu[i], mu[i], prod);
    }
    finish = clock();
    printf("Partial proof_gen costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    mpz_clear(r_i);
    return 0;
}

int aggregation(void)
{
    element_init_G1(P, pairing);
    element_init_Zr(Lambda, pairing);
    element_set1(P);
    element_set0(Lambda);
    start = clock();
    for (int i = 0; i < VAR_M; i++) {
        element_mul(P, P, sigma[i]);
        element_add(Lambda, Lambda, mu[i]);
    }
    finish = clock();
    printf("Aggregation costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
    return 0;
}

int self_verify(void)
{
    char path_str[64];
    char hash_key[64];
    unsigned char(*tau)[32] = malloc(VAR_M * 32 * sizeof(unsigned char));
    get_tau(tau);
    element_t g1_tmp2, g1_tmp3;
    element_init_G1(g1_tmp2, pairing);
    element_init_G1(g1_tmp3, pairing);
    read_g1_from_file(g1, PRA_BASE, "g1");
    read_g1_from_file(g, PRA_BASE, "g");
    read_g1_from_file(z, PRA_BASE, "z");

    element_set1(g1_tmp2);
    for (int i = 0; i < VAR_M; i++) {
        element_to_bytes_compressed(g1_str, R[i]);
        mpz_import(pz_tmp, g1_size, 1, sizeof(unsigned char), 0, 0, g1_str);
        element_pow_mpz(g1_tmp, R[i], pz_tmp);
        element_mul(g1_tmp2, g1_tmp2, g1_tmp);
    }
    element_mul(P, P, g1_tmp2);

    element_set1(g1_tmp3);
    for (int j = 0; j < VAR_S; j++) {
        element_set1(g1_tmp2);
        for (int i = 0; i < VAR_M; i++) {
            memset(hash_key, 0, 64);
            memcpy(hash_key, tau[i], 32);
            sprintf(hash_key + 32, "%s%d%s", "||", i, "||");
            sprintf(hash_key + strlen(hash_key), "%d%c%c%c", J[j], '\0', '\0', '\0');
            H1(g1_tmp, g, pz_tmp, hash_key);
            element_mul(g1_tmp2, g1_tmp2, g1_tmp);
        }
        element_pow_zn(g1_tmp2, g1_tmp2, u[j]);
        element_mul(g1_tmp3, g1_tmp3, g1_tmp2);
    }
    element_pow_zn(g, g, Lambda);
    element_mul(g1_tmp3, g1_tmp3, g);

    element_t temp_left, temp_right;
    element_init_GT(temp_left, pairing);
    element_init_GT(temp_right, pairing);

    element_pairing(temp_left, P, g1);
    element_pairing(temp_right, g1_tmp3, z);

    // element_printf("%B\n", temp_left);
    // element_printf("%B\n", temp_right);

    if (!element_cmp(temp_left, temp_right)) {
        printf("Integrity verifies\n");
    } else {
        printf("*BUG* Integrity does not verify *BUG*\n");
    }
    finish = clock();
    printf("Verify costs %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);

    element_clear(g1_tmp2);
    element_clear(g1_tmp3);
    element_clear(temp_left);
    element_clear(temp_right);
    free(tau);
    return 0;
}

int clear_vars(void)
{
    element_clear(g);
    element_clear(g1);
    element_clear(alpha);
    element_clear(y);
    element_clear(z);

    element_clear(sigma_ij);
    element_clear(g1_tmp);

    mpz_clear(pz_tmp);
    mpz_clear(b_ij);

    pbc_free(g1_str);

    free(J);

    for (int j = 0; j < VAR_S; j++) {
        element_clear(u[j]);
    }
    free(u);

    for (int i = 0; i < VAR_M; i++) {
        element_clear(sigma[i]);
        element_clear(mu[i]);
        element_clear(R[i]);
    }
    free(sigma);
    free(mu);
    free(R);

    element_clear(P);
    element_clear(Lambda);
    pairing_clear(pairing);
    return 0;
}

int get_component(char (*component)[strlen(FILE_NAME) + 5])
{
    char *extension = strrchr(FILE_NAME, '.');
    long fname_len = extension ? extension - FILE_NAME : strlen(FILE_NAME);
    for (int i = 0; i < VAR_M - REDUNDANCY; i++) {
        memcpy(component[i], FILE_NAME, fname_len + 1);
        sprintf(component[i] + fname_len, "_k%02d%s", i + 1, extension);
    }
    for (int i = VAR_M - REDUNDANCY, j = 0; i < VAR_M; i++, j++) {
        memcpy(component[i], FILE_NAME, fname_len + 1);
        sprintf(component[i] + fname_len, "_m%02d%s", j + 1, extension);
    }

    return 0;
}
int get_component_name(char **component, int index)
{
    *component = (char *)malloc(strlen(FILE_NAME) + 5);
    memset(*component, 0, strlen(FILE_NAME) + 5);
    char *extension = strrchr(FILE_NAME, '.');
    long fname_len = extension ? extension - FILE_NAME : strlen(FILE_NAME);
    memcpy(*component, FILE_NAME, fname_len + 1);
    if (index < VAR_M - REDUNDANCY) {
        sprintf(*component + fname_len, "_k%02d%s", index + 1, extension);
    } else {
        sprintf(*component + fname_len, "_m%02d%s", index + 1 + REDUNDANCY - VAR_M, extension);
    }
    return 0;
}
int get_tau(unsigned char (*tau)[32])
{
    char path_str[64];
    FILE *fp;
    char *component;
    for (int i = 0; i < VAR_M; i++) {
        memset(path_str, 0, 64);
        get_component_name(&component, i);
        sprintf(path_str, "%s%s", COM_BASE, component);
        if ((fp = fopen(path_str, "r")) == NULL) {
            printf("Fail to open file *%s*\n", path_str);
            exit(0);
        }
        free(component);
        get_sha256(path_str, tau[i]);
        fclose(fp);
    }
    return 0;
}

int write_g1_to_file(element_t gg, char *base, char *name)
{
    char path_str[64] = {0};
    sprintf(path_str, "%s%s", base, name);
    element_to_bytes_compressed(g1_str, gg);
    write_str_to_file(g1_str, g1_size, path_str);
    return 0;
}

int read_g1_from_file(element_t gg, char *base, char *name)
{
    char path_str[64] = {0};
    sprintf(path_str, "%s%s", base, name);
    read_str_from_file(g1_str, g1_size, path_str);
    element_from_bytes_compressed(gg, g1_str);
    return 0;
}
