#define DHIR_BENCH_NAME "lud"
#include "dhir_bench.h"

extern "C" {
void lud(int32_t,
         float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_lud(int size, float *a);
}

#include <string>

static int matrix_size = 0;
static float *a = nullptr;
static float *a_ref = nullptr;
static float *a_init = nullptr;

static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "r");
    if (fp) return fp;

    // Try relative to vendor/rodinia/rodinia_3.1/data/lud/
    std::string p1 = std::string("vendor/rodinia/rodinia_3.1/data/lud/") + path;
    fp = fopen(p1.c_str(), "r");
    if (fp) return fp;

    return nullptr;
}

static void bench_alloc(void) {
    const char *infile = getenv("LUD_INPUT");
    if (!infile) infile = getenv("LUD_DATASET");
    if (!infile) infile = "vendor/rodinia/rodinia_3.1/data/lud/2048.dat";

    FILE *fp = open_resolved(infile);
    if (!fp) {
        fprintf(stderr, "[lud] Cannot open dataset file: %s\n", infile);
        exit(1);
    }

    int file_size = 0;
    if (fscanf(fp, "%d\n", &file_size) != 1 || file_size <= 0) {
        fprintf(stderr, "[lud] Error reading matrix size from %s\n", infile);
        fclose(fp);
        exit(1);
    }
    matrix_size = file_size;

    long n = (long)matrix_size * matrix_size;
    a_init = dhir_alloc(n);
    a = dhir_alloc(n);
    a_ref = dhir_alloc(n);

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            if (fscanf(fp, "%f", &a_init[(long)i * matrix_size + j]) != 1) {
                fprintf(stderr, "[lud] Error reading matrix at (%d, %d)\n", i, j);
                fclose(fp);
                exit(1);
            }
        }
    }
    fclose(fp);

    for (long i = 0; i < n; i++) {
        a[i] = a_init[i];
        a_ref[i] = a_init[i];
    }
}

static void bench_call(void) {
    lud(matrix_size, MR2(a, matrix_size, matrix_size));
}

static void bench_reference(void) {
    ref_lud(matrix_size, a_ref);
}

static int bench_check(void) {
    int errors = dhir_compare("a", a, a_ref, (long)matrix_size * matrix_size, DHIR_EPS);

    return errors;
}

static void bench_free(void) {
    if (a) free(a);
    if (a_ref) free(a_ref);
    if (a_init) free(a_init);
    a = a_ref = a_init = nullptr;
}

