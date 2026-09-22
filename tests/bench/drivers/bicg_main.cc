// bicg: s = A^T * r ; q = A * p
#define DHIR_BENCH_NAME "bicg"
#include "dhir_bench.h"

extern "C" void kernel_bicg(int32_t, int32_t,
                            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t,
                            float *, float *, int64_t, int64_t, int64_t);

static float *A, *s, *q, *p, *r, *s_ref, *q_ref;

static void bench_alloc(void) {
    A = dhir_alloc((long)DHIR_M * DHIR_N);
    s = dhir_alloc(DHIR_N);
    q = dhir_alloc(DHIR_M);
    p = dhir_alloc(DHIR_N);
    r = dhir_alloc(DHIR_M);
    s_ref = dhir_alloc(DHIR_N);
    q_ref = dhir_alloc(DHIR_M);

    dhir_fill(A, (long)DHIR_M * DHIR_N, 1);
    dhir_fill(p, DHIR_N, 7);
    dhir_fill(r, DHIR_M, 13);
    dhir_zero(s, DHIR_N);
    dhir_zero(q, DHIR_M);
}

static void bench_call(void) {
    kernel_bicg(DHIR_M, DHIR_N,
                MR2(A, DHIR_M, DHIR_N),
                MR1(s, DHIR_N),
                MR1(q, DHIR_M),
                MR1(p, DHIR_N),
                MR1(r, DHIR_M));
}

static void bench_reference(void) {
    for (int i = 0; i < DHIR_N; ++i) s_ref[i] = 0.0f;
    for (int i = 0; i < DHIR_M; ++i) {
        q_ref[i] = 0.0f;
        for (int j = 0; j < DHIR_N; ++j) {
            s_ref[j] += r[i] * A[(long)i * DHIR_N + j];
            q_ref[i] += A[(long)i * DHIR_N + j] * p[j];
        }
    }
}

static int bench_check(void) {
    int errors = dhir_compare("s", s, s_ref, DHIR_N, DHIR_EPS);
    errors += dhir_compare("q", q, q_ref, DHIR_M, DHIR_EPS);
    return errors;
}

static void bench_free(void) {
    free(A); free(s); free(q); free(p); free(r);
    free(s_ref); free(q_ref);
}
