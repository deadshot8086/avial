// Unified C++ driver framework for Parboil Extracted MLIR Benchmarks
// Manages MLIR MemRef struct descriptors, MPI lifecycle, and tolerance checking.
#pragma once

#include <mpi.h>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef DHIR_BENCH_NAME
#error "define DHIR_BENCH_NAME before including dhir_bench.h"
#endif

// Default tolerance. Kernels that accumulate over long reductions or floats raise it if needed.
#ifndef DHIR_EPS
#define DHIR_EPS 1e-3f
#endif

// ---------------------------------------------------------------------------
// memref descriptor ABI
//
// MLIR lowers a memref argument to:
// (allocated_ptr, aligned_ptr, offset, sizes..., strides...)
// ---------------------------------------------------------------------------
#define MR1(p, d0) \
    (p), (p), (int64_t)0, (int64_t)(d0), (int64_t)1

#define MR2(p, d0, d1) \
    (p), (p), (int64_t)0, (int64_t)(d0), (int64_t)(d1), (int64_t)(d1), (int64_t)1

#define MR3(p, d0, d1, d2) \
    (p), (p), (int64_t)0, (int64_t)(d0), (int64_t)(d1), (int64_t)(d2), \
        (int64_t)((d1) * (d2)), (int64_t)(d2), (int64_t)1

#define MR4(p, d0, d1, d2, d3) \
    (p), (p), (int64_t)0, (int64_t)(d0), (int64_t)(d1), (int64_t)(d2), (int64_t)(d3), \
        (int64_t)((d1) * (d2) * (d3)), (int64_t)((d2) * (d3)), (int64_t)(d3), (int64_t)1

// ---------------------------------------------------------------------------
// allocation & utility helpers
// ---------------------------------------------------------------------------
static inline float *dhir_alloc(long n) {
    float *p = (float *)aligned_alloc(64, ((size_t)n * sizeof(float) + 63) / 64 * 64);
    if (!p) {
        fprintf(stderr, "%s: allocation of %ld floats failed\n", DHIR_BENCH_NAME, n);
        exit(2);
    }
    return p;
}

static inline double *dhir_alloc_double(long n) {
    double *p = (double *)aligned_alloc(64, ((size_t)n * sizeof(double) + 63) / 64 * 64);
    if (!p) {
        fprintf(stderr, "%s: allocation of %ld doubles failed\n", DHIR_BENCH_NAME, n);
        exit(2);
    }
    return p;
}

static inline int *dhir_alloc_int(long n) {
    int *p = (int *)aligned_alloc(64, ((size_t)n * sizeof(int) + 63) / 64 * 64);
    if (!p) {
        fprintf(stderr, "%s: allocation of %ld ints failed\n", DHIR_BENCH_NAME, n);
        exit(2);
    }
    return p;
}

static inline int64_t *dhir_alloc_int64(long n) {
    int64_t *p = (int64_t *)aligned_alloc(64, ((size_t)n * sizeof(int64_t) + 63) / 64 * 64);
    if (!p) {
        fprintf(stderr, "%s: allocation of %ld int64s failed\n", DHIR_BENCH_NAME, n);
        exit(2);
    }
    return p;
}

static inline void dhir_zero(float *p, long n) {
    for (long i = 0; i < n; ++i) p[i] = 0.0f;
}

static inline void dhir_zero_double(double *p, long n) {
    for (long i = 0; i < n; ++i) p[i] = 0.0;
}

static inline void dhir_zero_int(int *p, long n) {
    for (long i = 0; i < n; ++i) p[i] = 0;
}

// ---------------------------------------------------------------------------
// verification comparison helpers
// ---------------------------------------------------------------------------
static inline int dhir_compare(const char *what, const float *got,
                               const float *want, long n, float eps) {
    int errors = 0;
    for (long i = 0; i < n; ++i) {
        float w = want[i], g = got[i];
        float tol = eps * (std::fabs(w) > 1.0f ? std::fabs(w) : 1.0f);
        if (std::isnan(g) || std::isnan(w) || !(std::fabs(g - w) <= tol)) {
            if (errors < 5)
                printf("  mismatch %s[%ld]: got %.6f want %.6f (diff: %.6f, tol: %.6f)\n",
                       what, i, g, w, std::fabs(g - w), tol);
            ++errors;
        }
    }
    if (errors > 5) printf("  ... and %d more in %s\n", errors - 5, what);
    return errors;
}

static inline int dhir_compare_double(const char *what, const double *got,
                                      const double *want, long n, double eps) {
    int errors = 0;
    for (long i = 0; i < n; ++i) {
        double w = want[i], g = got[i];
        double tol = eps * (std::fabs(w) > 1.0 ? std::fabs(w) : 1.0);
        if (std::isnan(g) || std::isnan(w) || !(std::fabs(g - w) <= tol)) {
            if (errors < 5)
                printf("  mismatch %s[%ld]: got %.6lf want %.6lf (diff: %.6lf, tol: %.6lf)\n",
                       what, i, g, w, std::fabs(g - w), tol);
            ++errors;
        }
    }
    if (errors > 5) printf("  ... and %d more in %s\n", errors - 5, what);
    return errors;
}

static inline int dhir_compare_int(const char *what, const int *got,
                                   const int *want, long n) {
    int errors = 0;
    for (long i = 0; i < n; ++i) {
        int w = want[i], g = got[i];
        if (g != w) {
            if (errors < 5)
                printf("  mismatch %s[%ld]: got %d want %d\n", what, i, g, w);
            ++errors;
        }
    }
    if (errors > 5) printf("  ... and %d more in %s\n", errors - 5, what);
    return errors;
}

static inline int dhir_compare_int64(const char *what, const int64_t *got,
                                     const int64_t *want, long n) {
    int errors = 0;
    for (long i = 0; i < n; ++i) {
        int64_t w = want[i], g = got[i];
        if (g != w) {
            if (errors < 5)
                printf("  mismatch %s[%ld]: got %ld want %ld\n", what, i, (long)g, (long)w);
            ++errors;
        }
    }
    if (errors > 5) printf("  ... and %d more in %s\n", errors - 5, what);
    return errors;
}

// ---------------------------------------------------------------------------
// Hooks supplied by the driver
// ---------------------------------------------------------------------------
static void bench_alloc(void);
static void bench_call(void);
static void bench_reference(void);
static int bench_check(void);
static void bench_free(void);

extern "C" int dhir_noop_mpi_init(void *, void *) { return 0; }
extern "C" int dhir_noop_mpi_finalize(void) { return 0; }

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank = 0, nranks = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    bench_alloc();

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();
    bench_call();
    double elapsed = MPI_Wtime() - t0;

    double max_elapsed = elapsed;
    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    int failed = 0;
    if (rank == 0) {
        bench_reference();
        int errors = bench_check();
        failed = errors ? 1 : 0;
        printf("RESULT kernel=%s ranks=%d seconds=%.6f status=%s errors=%d\n",
               DHIR_BENCH_NAME, nranks, max_elapsed, failed ? "FAIL" : "PASS",
               errors);
    }

    MPI_Bcast(&failed, 1, MPI_INT, 0, MPI_COMM_WORLD);
    bench_free();
    MPI_Finalize();
    return failed;
}
