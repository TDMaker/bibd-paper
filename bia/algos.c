#include "algos.h"

int E[v][k + 1];
int F[v][v];
int D[v][k + 1];
mpz_t *PP[v];
mpz_t *CP;
size_t *u_s;

int bibd_init(void)
{
    generate_es(E);
    generate_from(E, F);
    generate_d(E, D);
    CP = (mpz_t *)malloc((v) * sizeof(mpz_t));
    for (int i = 0; i < v; i++) {
        PP[i] = (mpz_t *)malloc((k + 1) * sizeof(mpz_t));
    }
    u_s = (size_t *)malloc(CHAL_AMOUNT * sizeof(size_t));
    return 0;
}
int store(void)
{
    clock_t start, finish;
    FILE *fp_read, *fp_write;
    char write_name[128];
    if ((fp_read = fopen(SOURCED_FILE_NAME, "r")) == NULL) {
        printf("Fail to open file *%s*\n", SOURCED_FILE_NAME);
        exit(0);
    }
    size_t component_real_size = 0;
    size_t index = 0;
    unsigned char *component = (unsigned char *)malloc(COMP_SIZE * sizeof(unsigned char));
    memset(component, 0, COMP_SIZE * sizeof(unsigned char));
    start = clock();
    while ((component_real_size = fread(component, 1, COMP_SIZE, fp_read)) > 0 && index < (v)) {
        for (int i = 0; i < k + 1; i++) {
            memset(write_name, 0, 128);
            sprintf(write_name, COMPONENT_PATH, k, D[index][i], index);
            write_str_to_file(component, component_real_size, write_name, fp_write);
        }
        index++;
        memset(component, 0, COMP_SIZE * sizeof(unsigned char));
    }
    finish = clock();
    fclose(fp_read);
    free(component);
    printf("%f seconds in Store.\n", (double)(finish - start) / CLOCKS_PER_SEC);
    return 0;
}
size_t challenge(void)
{
    FILE *fp;
    size_t seed;
    read_str_from_file((unsigned char *)&seed, sizeof(size_t), "/dev/urandom", fp);
    psd_permute(u_s, CHAL_AMOUNT, seed, COMP_SIZE / BLK_SIZE);
    return seed;
}
int pproof_gen(size_t seed)
{
    FILE *fp;
    clock_t start, finish;
    char read_name[128];
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    char modular_str[SECURITY_LEVEL + 1] = {0};
    init(modular_str, 'c', "1", SECURITY_LEVEL);
    mpz_t *nu_s = (mpz_t *)malloc(CHAL_AMOUNT * sizeof(mpz_t));
    mpz_t im_sum, modular;
    mpz_init(im_sum);
    mpz_init_set_str(modular, modular_str, 2);
    unsigned char *blk_buf = (unsigned char *)malloc(BLK_SIZE * sizeof(unsigned char));
    for (int i = 0; i < CHAL_AMOUNT; i++) {
        mpz_init(nu_s[i]);
        mpz_urandomb(nu_s[i], state, SECURITY_LEVEL);
    }
    start = clock();
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < k + 1; j++) {
            mpz_init(PP[i][j]);
            memset(blk_buf, 0, BLK_SIZE * sizeof(unsigned char));
            memset(read_name, 0, 128);
            sprintf(read_name, COMPONENT_PATH, k, i, (size_t)E[i][j]);
            if ((fp = fopen(read_name, "r")) == NULL) {
                printf("Fail to open file *%s*\n", read_name);
                exit(0);
            }
            for (int m = 0; m < CHAL_AMOUNT; m++) {
                fseek(fp, u_s[m] * BLK_SIZE, SEEK_SET);
                fread(blk_buf, BLK_SIZE, 1, fp);
                mpz_import(im_sum, BLK_SIZE, 1, sizeof(unsigned char), 0, 0, blk_buf);
                mpz_mod(im_sum, im_sum, modular);
                mpz_mul(im_sum, im_sum, nu_s[m]);
                mpz_add(PP[i][j], PP[i][j], im_sum);
                mpz_mod(PP[i][j], PP[i][j], modular);
            }
            fclose(fp);
        }
    }
    finish = clock();
    mpz_clear(im_sum);
    mpz_clear(modular);
    for (int i = 0; i < CHAL_AMOUNT; i++) {
        mpz_clear(nu_s[i]);
    }
    free(nu_s);
    free(blk_buf);
    printf("%f seconds in Partial Proofs Generation.\n", (double)(finish - start) / CLOCKS_PER_SEC);
    return 0;
}

int cproof_gen(void)
{
    int csp;
    clock_t start, finish;
    start = clock();
    for (int i = 0; i < v; i++) {
        mpz_init(CP[i]);
        for (int j = 0; j < v; j++) {
            csp = F[i][j];
            for (int index_of_com = 0; index_of_com < k + 1; index_of_com++) {
                if (j == E[csp][index_of_com]) {
                    mpz_add(CP[i], CP[i], PP[csp][index_of_com]);
                    break;
                }
            }
        }
    }
    finish = clock();
    printf("%f seconds in Complete Proofs Generation.\n", (double)(finish - start) / CLOCKS_PER_SEC);
    return 0;
}
int verify()
{
#ifdef TEST
    for (int i = 0; i < v; i++) {
        gmp_printf("CP[%02d]=%Zd\n", i, CP[i]);
    }
#endif
    clock_t start, finish;
    start = clock();
    int results[v] = {0};
    for (int i = 0; i < v; i++) {
        for (int j = 1; j < v; j++) {
            if (mpz_cmp(CP[i], CP[(i + j) % (v)]) == 0) {
                results[i]++;
            }
        }
    }
    int valid = find_max(results, v);
    if (valid == v - 1) {
        finish = clock();
        printf("All the components are constant.\n");
        printf("%f seconds in Verification.\n", (double)(finish - start) / CLOCKS_PER_SEC);
        return 0;
    }

    printf("The max is %d\n", valid);

    int S[v][v];

    for (int row_s = 0; row_s < v; row_s++) {
        for (int col_s = 0; col_s < v; col_s++) {
            S[row_s][col_s] = 0;
        }
    }

    for (int i = 0; i < v; i++) {
        if (results[i] != valid) {
            for (int j = 0; j < v; j++) {
                S[F[i][j]][j]++;
            }
        }
    }

    for (int row_s = 0; row_s < v; row_s++) {
        for (int col_s = 0; col_s < v; col_s++) {
            if (S[row_s][col_s] != 0) {
                // printf("S[%d][%d] is %d\n", row_s, col_s, S[row_s][col_s]);
                for (int row_f = 0; row_f < v; row_f++) {
                    if (results[row_f] == valid && F[row_f][col_s] == row_s) {
                        S[row_s][col_s] = 0;
                    }
                }
            }
        }
    }

    for (int row_s = 0; row_s < v; row_s++) {
        for (int col_s = 0; col_s < v; col_s++) {
            if (S[row_s][col_s] != 0) {
                printf("CSP%02d's Component%02d may be corrupted.\n", row_s, col_s);
            }
        }
    }
    finish = clock();
    printf("%f seconds in Verification.\n", (double)(finish - start) / CLOCKS_PER_SEC);

    return 0;
}
int damage(const char *filename, long pos, const char *value)
{
    FILE *fp;
    if ((fp = fopen(filename, "r+")) == NULL) {
        printf("Fail to open file *%s*\n", filename);
        exit(0);
    }
    fseek(fp, pos, SEEK_SET);
    fwrite(value, sizeof(char), strlen(value), fp);
    fclose(fp);
    return 0;
}
int clear(void)
{
    for (int i = 0; i < v; i++) {
        mpz_clear(CP[i]);
    }
    free(CP);

    for (int i = 0; i < v; i++) {
        for (int j = 0; j < k + 1; j++) {
            mpz_clear(PP[i][j]);
        }
        free(PP[i]);
    }

    free(u_s);
    return 0;
}