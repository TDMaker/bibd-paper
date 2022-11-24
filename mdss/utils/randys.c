#include <pbc/pbc.h>
#include <string.h>

// https://benpfaff.org/writings/clc/shuffle.html
/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void psd_permute(int *array, int n, unsigned int seed, int N)
{
    n = N > n ? n : N;
    srand(seed);
    int *_array = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++)
        _array[i] = i;

    for (int i = 0; i < N - 1; i++) {
        int j = i + rand() / (RAND_MAX / (N - i) + 1);
        int t = _array[j];
        _array[j] = _array[i];
        _array[i] = t;
    }
    memcpy(array, _array, sizeof(int) * n);
    free(_array);
}

int pairing_init(pairing_t pairing, char *pairing_param_file_name)
{
    FILE *fp;
    char param[1024];
    if ((fp = fopen(pairing_param_file_name, "r")) == NULL) {
        printf("Pairing param file *%s* open error!\n", pairing_param_file_name);
        return -1;
    }
    size_t count = fread(param, 1, 1024, fp);
    fclose(fp);
    if (!count) {
        puts("pairing param file read failed!");
        return -2;
    }
    pairing_init_set_buf(pairing, param, count);
    return 0;
}

void write_str_to_file(unsigned char *_str, size_t _size, const char *file_name)
{
    FILE *fp;
    if ((fp = fopen(file_name, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_name);
        exit(0);
    }
    fwrite(_str, 1, _size, fp);
    fclose(fp);
}
void read_str_from_file(unsigned char *_str, size_t _size, const char *file_name)
{
    FILE *fp;
    if ((fp = fopen(file_name, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_name);
        exit(0);
    }
    fread(_str, 1, _size, fp);
    fclose(fp);
}
void write_int_to_file(int *_num, size_t _size, const char *file_name)
{
    FILE *fp;
    if ((fp = fopen(file_name, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_name);
        exit(0);
    }
    fwrite(_num, 1, _size, fp);
    fclose(fp);
}
void read_int_from_file(int *num, size_t _size, const char *file_name)
{
    FILE *fp;
    if ((fp = fopen(file_name, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_name);
        exit(0);
    }
    fread(num, 1, _size, fp);
    fclose(fp);
}
int get_sha256_plain(char *dest, unsigned char *hash_val)
{
    for (int ii = 0; ii < 32; ii++) {
        sprintf(dest + ii * 2, "%02x", hash_val[ii]);
    }
    return 0;
}

void H1(element_t g1, char *str, int str_len)
{
    element_from_hash(g1, str, str_len);
}

void H2(element_t zr, element_t g1, int g1_size)
{
    unsigned char *g1_str = pbc_malloc(g1_size);
    element_to_bytes_compressed(g1_str, g1);
    element_from_hash(zr, g1_str, g1_size);
    pbc_free(g1_str);
}
void H3(element_t zr_op, element_t zr_ip, char *str, int str_len)
{
    element_from_hash(zr_op, str, str_len);
    element_add(zr_op, zr_op, zr_ip);
}
void H4(element_t g1, char *str, int str_len) __attribute__((weak, alias("H1")));