// floyd_warshall: all-pairs shortest paths
//
// path is read-modify-written, so the reference needs its initial contents.
#define DHIR_BENCH_NAME "floyd_warshall"
#include "dhir_bench.h"

extern "C" void kernel_floyd_warshall(int32_t,
                                      float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

static float *path, *path_init, *path_ref;

static void bench_alloc(void) {
    path = dhir_alloc((long)DHIR_N * DHIR_N);
    path_init = dhir_alloc((long)DHIR_N * DHIR_N);
    path_ref = dhir_alloc((long)DHIR_N * DHIR_N);

    dhir_fill(path, (long)DHIR_N * DHIR_N, 1);
    for (long i = 0; i < (long)DHIR_N * DHIR_N; ++i) path_init[i] = path[i];
}

static void bench_call(void) {
    kernel_floyd_warshall(DHIR_N, MR2(path, DHIR_N, DHIR_N));
}

static void bench_reference(void) {
    for (long i = 0; i < (long)DHIR_N * DHIR_N; ++i)
        path_ref[i] = path_init[i];

    for (int k = 0; k < DHIR_N; ++k)
        for (int i = 0; i < DHIR_N; ++i)
            for (int j = 0; j < DHIR_N; ++j) {
                float sum = path_ref[(long)i * DHIR_N + k] + path_ref[(long)k * DHIR_N + j];
                if (path_ref[(long)i * DHIR_N + j] > sum)
                    path_ref[(long)i * DHIR_N + j] = sum;
            }
}

static int bench_check(void) {
    return dhir_compare("path", path, path_ref, (long)DHIR_N * DHIR_N, DHIR_EPS);
}

static void bench_free(void) {
    free(path); free(path_init); free(path_ref);
}
