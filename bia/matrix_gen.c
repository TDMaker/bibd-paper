#include "./utils/settings.h"
#include <stdio.h>
#include <string.h>
#define CEIL(f, n) (f) / (n) + ((f) % (n) > 0 ? 1 : 0)
int generate_es(int E[][k + 1])
{
    int D[k * k][k + 1];
    int B[v][k + 1];
    // int E[k * k + k + 1][k + 1];

    for (int i = 0; i < k + 1; i++) {
        B[i][0] = 0;
        for (int j = 1; j < k + 1; j++) {
            B[i][j] = i * k + j;
        }
    }

    for (int i = k + 1; i < v; i++) {
        B[i][0] = (i - 1) / k;
        for (int j = 1; j < k + 1; j++) {
            B[i][j] = j * k + 1 + (i - j + (j - 1) * ((i - 1) / k)) % k;
        }
    }
#ifdef TEST
    for (int i = 0; i < v; i++) {
        printf("B[%d] = {", i);
        for (int j = 0; j < k + 1; j++) {
            printf("%2d,", B[i][j]);
        }
        printf("\b}\n");
    }
#endif
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < k + 1; j++) {
            E[i][j] = -1;
        }
    }

    memcpy(E[0], B[0], (k + 1) * sizeof(int));

    for (int i = 1; i < k + 1; i++) {
        memcpy(E[(i + 1) * k], B[i], (k + 1) * sizeof(int));
    }
    for (int i = k + 1; i < v; i++) {
        if (B[i][CEIL(i, k) - 1] % k == 0) {
            memcpy(E[CEIL(i, k) - 1], B[i], (k + 1) * sizeof(int));
        } else {
            memcpy(E[B[i][CEIL(i, k) - 1]], B[i], (k + 1) * sizeof(int));
        }
    }
#ifdef TEST
    printf("\n");
    for (int i = 0; i < v; i++) {
        printf("E[%d] = {", i);
        for (int j = 0; j < k + 1; j++) {
            printf("%2d,", E[i][j]);
        }
        printf("\b}\n");
    }
#endif
    return 0;
}
int generate_from(int E[][k + 1], int F[][v])
{
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < v; j++) {
            F[i][j] = -1;
        }
    }

    for (int i = 0; i < v; i++) {
        F[i][i] = i;
        for (int j = 0; j < k + 1; j++) {
            for (int t = 1; t < k + 1; t++) {
                F[E[i][j]][E[i][(j + t) % (k + 1)]] = i;
            }
        }
    }
#ifdef TEST
    printf("\nMatrix F\n");
    for (int i = 0; i < v; i++) {
        printf("CS%02d: {", i);
        for (int j = 0; j < v; j++) {
            printf("%2d, ", F[i][j]);
        }
        printf("\b\b}\n");
    }
#endif
    return 0;
}

int generate_d(int E[][k + 1], int D[][k + 1])
{
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < k + 1; j++) {
            D[i][j] = -1;
        }
    }
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < k + 1; j++) {
            for (int l = 0; l < k + 1; l++) {
                if (D[E[i][j]][l] == -1) {
                    D[E[i][j]][l] = i;
                    break;
                } else {
                    continue;
                }
            }
        }
    }
#ifdef TEST
    printf("\n");
    for (int i = 0; i < v; i++) {
        printf("D[%d] = {", i);
        for (int j = 0; j < k + 1; j++) {
            printf("%2d,", D[i][j]);
        }
        printf("\b}\n");
    }
#endif
    return 0;
}