#include "utils.h"
void write_str_to_file(unsigned char *_str, size_t _size, char *file_name, FILE *fp)
{
    if ((fp = fopen(file_name, "w")) == NULL) {
        printf("Fail to open file *%s*\n", file_name);
        exit(0);
    }
    fwrite(_str, 1, _size, fp);
    fclose(fp);
}
void read_str_from_file(unsigned char *_str, size_t _size, char *file_name, FILE *fp)
{
    if ((fp = fopen(file_name, "r")) == NULL) {
        printf("Fail to open file *%s*\n", file_name);
        exit(0);
    }
    fread(_str, 1, _size, fp);
    fclose(fp);
}
void psd_permute(size_t *array, size_t n, unsigned int seed, size_t N)
{
    n = N > n ? n : N;
    srand(seed);
    size_t *_array = malloc(N * sizeof(size_t));
    for (size_t i = 0; i < N; i++)
        _array[i] = i;

    for (size_t i = 0; i < N - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (N - i) + 1);
        size_t t = _array[j];
        _array[j] = _array[i];
        _array[i] = t;
    }
    memcpy(array, _array, sizeof(size_t) * n);
    free(_array);
}

void psd_func(size_t *array, size_t n, unsigned int seed, size_t q)
{
    srand(seed);
    for (size_t i = 0; i < n; i++) {
        array[i] = rand() % q;
    }
}
int init(void *addr, char type, const char *value, int length)
{
    int num;
    switch (type) {
    case 'c':
        for (int i = 0; i < length; i++) {
            ((char *)addr)[i] = value[0];
        }
        break;
    case 'i':
        num = atoi(value);
        for (int i = 0; i < length; i++) {
            ((int *)addr)[i] = num;
        }
    }
    return 0;
}
int find_max_index(int *addr, int length)
{
    int index = 0;
    int max = addr[index];
    for (int i = 1; i < length; i++) {
        if (addr[i] > addr[index]) {
            index = i;
        }
    }
    return index;
}
int find_max(int *addr, int length)
{
    int max = -1;
    for (int i = 0; i < length; i++) {
        if (addr[i] > max) {
            max = addr[i];
        }
    }
    return max;
}
int is_in(int value, int *addr, int length)
{
    for (int i = 0; i < length; i++) {
        if (addr[i] == value) {
            return 1;
        }
    }
    return 0;
}

int remove_ele(int value, int *addr, int length)
{
    for (int i = 0; i < length; i++) {
        if (addr[i] == value) {
            addr[i] = -1;
        }
    }
    return 0;
}