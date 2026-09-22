#define DHIR_BENCH_NAME "gaussian"
#include "dhir_bench.h"

#include <string>

extern "C" {
void gaussian(
    int32_t Size,
    float *m_allocated, float *m_aligned, int64_t m_offset, int64_t m_size0, int64_t m_size1, int64_t m_stride0, int64_t m_stride1,
    float *a_allocated, float *a_aligned, int64_t a_offset, int64_t a_size0, int64_t a_size1, int64_t a_stride0, int64_t a_stride1,
    float *b_allocated, float *b_aligned, int64_t b_offset, int64_t b_size, int64_t b_stride,
    float *x_allocated, float *x_aligned, int64_t x_offset, int64_t x_size, int64_t x_stride
);

void ref_gaussian(int Size, float *m, float *a, float *b, float *x);
}

static int Size = 0;
static float *a = nullptr, *a_ref = nullptr, *a_init = nullptr;
static float *m = nullptr, *m_ref = nullptr;
static float *b = nullptr, *b_ref = nullptr, *b_init = nullptr;
static float *x = nullptr, *x_ref = nullptr;

static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "r");
    if (fp) return fp;

    // Try relative to vendor/rodinia/rodinia_3.1/data/gaussian/
    std::string p1 = std::string("vendor/rodinia/rodinia_3.1/data/gaussian/") + path;
    fp = fopen(p1.c_str(), "r");
    if (fp) return fp;

    return nullptr;
}

static void bench_alloc(void) {
    const char *infile = getenv("GAUSSIAN_INPUT");
    if (!infile) infile = getenv("GAUSSIAN_DATASET");
    if (!infile) infile = "vendor/rodinia/rodinia_3.1/data/gaussian/matrix1024.txt";

    FILE *fp = open_resolved(infile);
    if (!fp) {
        fprintf(stderr, "[gaussian] Cannot open dataset file: %s\n", infile);
        exit(1);
    }

    int file_size = 0;
    if (fscanf(fp, "%d", &file_size) != 1 || file_size <= 0) {
        fprintf(stderr, "[gaussian] Error reading Size from %s\n", infile);
        fclose(fp);
        exit(1);
    }
    Size = file_size;

    long n = (long)Size * Size;
    a_init = dhir_alloc(n);
    a = dhir_alloc(n);
    a_ref = dhir_alloc(n);

    m = dhir_alloc(n);
    m_ref = dhir_alloc(n);

    b_init = dhir_alloc(Size);
    b = dhir_alloc(Size);
    b_ref = dhir_alloc(Size);

    x = dhir_alloc(Size);
    x_ref = dhir_alloc(Size);

    for (int i = 0; i < Size; i++) {
        for (int j = 0; j < Size; j++) {
            if (fscanf(fp, "%f", &a_init[(long)i * Size + j]) != 1) {
                fprintf(stderr, "[gaussian] Error reading matrix element (%d, %d)\n", i, j);
                fclose(fp);
                exit(1);
            }
        }
    }
    for (int i = 0; i < Size; i++) {
        if (fscanf(fp, "%f", &b_init[i]) != 1) {
            fprintf(stderr, "[gaussian] Error reading vector element b[%d]\n", i);
            fclose(fp);
            exit(1);
        }
    }
    fclose(fp);

    for (long i = 0; i < n; i++) {
        a[i] = a_init[i];
        a_ref[i] = a_init[i];
        m[i] = 0.0f;
        m_ref[i] = 0.0f;
    }
    for (int i = 0; i < Size; i++) {
        b[i] = b_init[i];
        b_ref[i] = b_init[i];
        x[i] = 0.0f;
        x_ref[i] = 0.0f;
    }
}

static void bench_call(void) {
    gaussian(Size,
             MR2(m, Size, Size),
             MR2(a, Size, Size),
             MR1(b, Size),
             MR1(x, Size));
}

static void bench_reference(void) {
    ref_gaussian(Size, m_ref, a_ref, b_ref, x_ref);
}

static int bench_check(void) {
    int errors = dhir_compare("x", x, x_ref, Size, DHIR_EPS);
    errors += dhir_compare("a", a, a_ref, (long)Size * Size, DHIR_EPS);
    errors += dhir_compare("m", m, m_ref, (long)Size * Size, DHIR_EPS);
    errors += dhir_compare("b", b, b_ref, Size, DHIR_EPS);

    return errors;
}

static void bench_free(void) {
    if (a) free(a);
    if (a_ref) free(a_ref);
    if (a_init) free(a_init);
    if (m) free(m);
    if (m_ref) free(m_ref);
    if (b) free(b);
    if (b_ref) free(b_ref);
    if (b_init) free(b_init);
    if (x) free(x);
    if (x_ref) free(x_ref);
    a = a_ref = a_init = nullptr;
    m = m_ref = nullptr;
    b = b_ref = b_init = nullptr;
    x = x_ref = nullptr;
}
