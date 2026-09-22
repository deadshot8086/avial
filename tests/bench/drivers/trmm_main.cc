// trmm: triangular matrix multiplication B = alpha * A^T * B (A lower triangular)
//
// B is read-modify-written, so the reference needs its initial contents.
#define DHIR_BENCH_NAME "trmm"
#include "dhir_bench.h"

extern "C" void kernel_trmm(int32_t, float,
                            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

static const float ALPHA = 1.5f;
static float *A, *B, *B_init, *B_ref;

static void bench_alloc(void) {
    A = dhir_alloc((long)DHIR_M * DHIR_M);
    B = dhir_alloc((long)DHIR_M * DHIR_N);
    B_init = dhir_alloc((long)DHIR_M * DHIR_N);
    B_ref = dhir_alloc((long)DHIR_M * DHIR_N);

    dhir_fill(A, (long)DHIR_M * DHIR_M, 1);
    dhir_fill(B, (long)DHIR_M * DHIR_N, 7);
    for (long i = 0; i < (long)DHIR_M * DHIR_M; ++i) A[i] /= DHIR_M;
    for (long i = 0; i < (long)DHIR_M * DHIR_N; ++i) B[i] /= DHIR_N;
    for (long i = 0; i < (long)DHIR_M * DHIR_N; ++i) B_init[i] = B[i];
}

static void bench_call(void) {
    kernel_trmm(DHIR_M, ALPHA,
                MR2(A, DHIR_M, DHIR_M),
                MR2(B, DHIR_M, DHIR_N));
}

static void bench_reference(void) {
    for (long idx = 0; idx < (long)DHIR_M * DHIR_N; ++idx)
        B_ref[idx] = B_init[idx];

    for (int i = 1; i < DHIR_M; ++i)
        for (int j = 0; j < DHIR_N; ++j)
            for (int k = 0; k < i; ++k)
                B_ref[(long)i * DHIR_N + j] += ALPHA * A[(long)i * DHIR_M + k] * B_ref[(long)j * DHIR_N + k];
}

static int bench_check(void) {
    return dhir_compare("B", B, B_ref, (long)DHIR_M * DHIR_N, DHIR_EPS);
}

static void bench_free(void) {
    free(A); free(B); free(B_init); free(B_ref);
}
