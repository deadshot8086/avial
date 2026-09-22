// symm: C = alpha*A*B + beta*C (A is symmetric)
//
// C is read-modify-written, so the reference needs its initial contents.
#define DHIR_BENCH_NAME "symm"
#include "dhir_bench.h"

extern "C" void kernel_symm(int32_t, int32_t, float, float,
                            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

static const float ALPHA = 1.5f, BETA = 1.2f;
static float *C, *A, *B, *C_init, *C_ref;

static void bench_alloc(void) {
    C = dhir_alloc((long)DHIR_M * DHIR_N);
    A = dhir_alloc((long)DHIR_M * DHIR_M);
    B = dhir_alloc((long)DHIR_M * DHIR_N);
    C_init = dhir_alloc((long)DHIR_M * DHIR_N);
    C_ref = dhir_alloc((long)DHIR_M * DHIR_N);

    dhir_fill(C, (long)DHIR_M * DHIR_N, 1);
    dhir_fill(A, (long)DHIR_M * DHIR_M, 7);
    dhir_fill(B, (long)DHIR_M * DHIR_N, 13);
    for (long i = 0; i < (long)DHIR_M * DHIR_N; ++i) C_init[i] = C[i];
}

static void bench_call(void) {
    kernel_symm(DHIR_M, DHIR_N, ALPHA, BETA,
                MR2(C, DHIR_M, DHIR_N),
                MR2(A, DHIR_M, DHIR_M),
                MR2(B, DHIR_M, DHIR_N));
}

static void bench_reference(void) {
    for (long idx = 0; idx < (long)DHIR_M * DHIR_N; ++idx)
        C_ref[idx] = C_init[idx];

    for (int i = 0; i < DHIR_M; ++i) {
        for (int j = 0; j < DHIR_N; ++j) {
            float acc = 0.0f;
            for (int k = 0; k < j - 1; ++k) {
                C_ref[(long)k * DHIR_N + j] += ALPHA * A[(long)k * DHIR_M + i] * B[(long)i * DHIR_N + j];
                acc += B[(long)k * DHIR_N + j] * A[(long)k * DHIR_M + i];
            }
            C_ref[(long)i * DHIR_N + j] = BETA * C_ref[(long)i * DHIR_N + j] +
                                          ALPHA * A[(long)i * DHIR_M + i] * B[(long)i * DHIR_N + j] +
                                          ALPHA * acc;
        }
    }
}

static int bench_check(void) {
    return dhir_compare("C", C, C_ref, (long)DHIR_M * DHIR_N, DHIR_EPS);
}

static void bench_free(void) {
    free(C); free(A); free(B); free(C_init); free(C_ref);
}
