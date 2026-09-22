// syrk: C = beta*C + alpha*A*A^T
//
// C is read-modify-written, so the reference needs its initial contents.
#define DHIR_BENCH_NAME "syrk"
#include "dhir_bench.h"

extern "C" void kernel_syrk(int32_t, int32_t, double, double,
                            double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t);

static const double ALPHA = 1.5, BETA = 0.75;
static double *C, *A, *C_init, *C_ref;

static void bench_alloc(void) {
    C = dhir_alloc((long)DHIR_N * DHIR_N);
    A = dhir_alloc((long)DHIR_N * DHIR_M);
    C_init = dhir_alloc((long)DHIR_N * DHIR_N);
    C_ref = dhir_alloc((long)DHIR_N * DHIR_N);

    dhir_fill(C, (long)DHIR_N * DHIR_N, 1);
    dhir_fill(A, (long)DHIR_N * DHIR_M, 7);
    for (long i = 0; i < (long)DHIR_N * DHIR_N; ++i) C_init[i] = C[i];
}

static void bench_call(void) {
    kernel_syrk(DHIR_N, DHIR_M, ALPHA, BETA,
                MR2(C, DHIR_N, DHIR_N),
                MR2(A, DHIR_N, DHIR_M));
}

static void bench_reference(void) {
    for (long i = 0; i < (long)DHIR_N * DHIR_N; ++i)
        C_ref[i] = C_init[i];

    for (int i = 0; i < DHIR_N; ++i)
        for (int j = 0; j <= i; ++j)
            C_ref[(long)i * DHIR_N + j] = C_init[(long)i * DHIR_N + j] * BETA;

    for (int i = 0; i < DHIR_N; ++i)
        for (int k = 0; k < DHIR_M; ++k)
            for (int j = 0; j <= i; ++j)
                C_ref[(long)i * DHIR_N + j] += ALPHA * A[(long)i * DHIR_M + k] * A[(long)j * DHIR_M + k];
}

static int bench_check(void) {
    return dhir_compare("C", C, C_ref, (long)DHIR_N * DHIR_N, DHIR_EPS);
}

static void bench_free(void) { free(C); free(A); free(C_init); free(C_ref); }
