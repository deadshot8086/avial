// seidel: T sweeps of a 2-D 9-point Gauss-Seidel stencil over [1, N-1)^2
#define DHIR_BENCH_NAME "seidel"
#include "dhir_bench.h"

extern "C" void kernel_seidel_2d(int32_t, int32_t,
                                 float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

static const double COEF = 9.000000e+00;

static float *A, *A_init, *A_ref;

static long n2(void) { return (long)DHIR_N * DHIR_N; }
#define AT(p, i, j) (p)[(long)(i) * DHIR_N + (j)]

static void bench_alloc(void) {
    A = dhir_alloc(n2());
    A_init = dhir_alloc(n2());
    A_ref = dhir_alloc(n2());

    dhir_fill(A, n2(), 1);
    for (long i = 0; i < n2(); ++i) A_init[i] = A[i];
}

static void bench_call(void) {
    kernel_seidel_2d(DHIR_T, DHIR_N,
                     MR2(A, DHIR_N, DHIR_N));
}

static void bench_reference(void) {
    for (long i = 0; i < n2(); ++i) A_ref[i] = A_init[i];
    for (int t = 0; t < DHIR_T; ++t) {
        for (int i = 1; i < DHIR_N - 1; ++i)
            for (int j = 1; j < DHIR_N - 1; ++j) {
                float s = AT(A_ref, i - 1, j - 1) + AT(A_ref, i - 1, j);
                s = s + AT(A_ref, i - 1, j + 1);
                s = s + AT(A_ref, i, j - 1);
                s = s + AT(A_ref, i, j);
                s = s + AT(A_ref, i, j + 1);
                s = s + AT(A_ref, i + 1, j - 1);
                s = s + AT(A_ref, i + 1, j);
                s = s + AT(A_ref, i + 1, j + 1);
                AT(A_ref, i, j) = (float)((double)s / COEF);
            }
    }
}

static int bench_check(void) {
    return dhir_compare("A", A, A_ref, n2(), DHIR_EPS);
}

static void bench_free(void) {
    free(A); free(A_init); free(A_ref);
}
