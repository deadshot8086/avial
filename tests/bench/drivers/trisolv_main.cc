// trisolv: triangular solver L * x = b
#define DHIR_BENCH_NAME "trisolv"
#include "dhir_bench.h"

extern "C" void kernel_trisolv(int32_t,
                               float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                               float *, float *, int64_t, int64_t, int64_t,
                               float *, float *, int64_t, int64_t, int64_t);

static float *L, *x, *b, *x_ref;

static void bench_alloc(void) {
    L = dhir_alloc((long)DHIR_N * DHIR_N);
    x = dhir_alloc(DHIR_N);
    b = dhir_alloc(DHIR_N);
    x_ref = dhir_alloc(DHIR_N);

    dhir_fill(L, (long)DHIR_N * DHIR_N, 1);
    dhir_fill(b, DHIR_N, 7);
    // Keep diagonal well away from zero to ensure stable division
    for (int i = 0; i < DHIR_N; ++i) L[(long)i * DHIR_N + i] += 2.0f;
    dhir_zero(x, DHIR_N);
}

static void bench_call(void) {
    kernel_trisolv(DHIR_N,
                   MR2(L, DHIR_N, DHIR_N),
                   MR1(x, DHIR_N),
                   MR1(b, DHIR_N));
}

static void bench_reference(void) {
    for (int i = 0; i < DHIR_N; ++i) {
        x_ref[i] = b[i];
        for (int j = 0; j < i; ++j)
            x_ref[i] -= L[(long)i * DHIR_N + j] * x_ref[j];
        x_ref[i] /= L[(long)i * DHIR_N + i];
    }
}

static int bench_check(void) {
    return dhir_compare("x", x, x_ref, DHIR_N, DHIR_EPS);
}

static void bench_free(void) {
    free(L); free(x); free(b); free(x_ref);
}
