// MPI driver for the extracted Parboil sgemm kernel.

#define DHIR_BENCH_NAME "sgemm"
#include "dhir_bench.h"

extern "C" {
void sgemm(int32_t, int32_t, int32_t, float, float,
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_sgemm(int m, int n, int k, float alpha, float beta,
               const float *A, const float *B, float *C);
}

#include <string>

static int m_dim = 0;  // rows of A / rows of C
static int n_dim = 0;  // rows of B^T / cols of C
static int k_dim = 0;  // shared dimension

static const float ALPHA = 1.0f;
static const float BETA = 0.0f;

static float *matA = nullptr;   // column-major, lda = m  -> viewed as a[k][m]
static float *matBT = nullptr;  // column-major, ldb = n  -> viewed as b[k][n]
static float *matC = nullptr;   // column-major, ldc = m  -> viewed as c[n][m]
static float *matC_ref = nullptr;

static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "r");
    if (fp) return fp;

    std::string p1 = std::string("vendor/parboil/datasets/sgemm/medium/input/") + path;
    fp = fopen(p1.c_str(), "r");
    return fp;
}


static float *read_col_major(const char *path, int *nr_row, int *nr_col) {
    FILE *fp = open_resolved(path);
    if (!fp) return nullptr;

    int r = 0, c = 0;
    if (fscanf(fp, "%d %d", &r, &c) != 2 || r <= 0 || c <= 0) {
        fclose(fp);
        return nullptr;
    }

    long total = (long)r * c;
    float *v = dhir_alloc(total);
    for (long i = 0; i < total; ++i) {
        if (fscanf(fp, "%f", &v[i]) != 1) {
            fprintf(stderr, "[sgemm] short read in %s at element %ld/%ld\n", path, i, total);
            fclose(fp);
            free(v);
            return nullptr;
        }
    }
    fclose(fp);

    *nr_row = r;
    *nr_col = c;
    return v;
}

static void synth_fallback(void) {
    const char *em = getenv("SGEMM_M");
    const char *en = getenv("SGEMM_N");
    const char *ek = getenv("SGEMM_K");
    m_dim = em ? atoi(em) : 128;
    n_dim = en ? atoi(en) : 160;
    k_dim = ek ? atoi(ek) : 96;
    if (m_dim <= 0) m_dim = 128;
    if (n_dim <= 0) n_dim = 160;
    if (k_dim <= 0) k_dim = 96;

    matA = dhir_alloc((long)m_dim * k_dim);
    matBT = dhir_alloc((long)n_dim * k_dim);

    srand(7);
    for (long i = 0; i < (long)m_dim * k_dim; ++i)
        matA[i] = (float)(rand() % 1000) / 1000.0f;
    for (long i = 0; i < (long)n_dim * k_dim; ++i)
        matBT[i] = (float)(rand() % 1000) / 1000.0f;

    fprintf(stderr, "[sgemm] dataset not found, using synthetic data "
                    "(m=%d n=%d k=%d, srand(7))\n", m_dim, n_dim, k_dim);
}

static void bench_alloc(void) {
    const char *fa = getenv("SGEMM_INPUT_A");
    const char *fb = getenv("SGEMM_INPUT_B");
    if (!fa) fa = "matrix1.txt";
    if (!fb) fb = "matrix2t.txt";

    int ar = 0, ac = 0, br = 0, bc = 0;
    matA = read_col_major(fa, &ar, &ac);
    if (matA) matBT = read_col_major(fb, &br, &bc);

    if (matA && matBT && ac == bc) {
        // A is m x k, B^T is n x k (Parboil binds nr_row -> matBcol = n).
        m_dim = ar;
        k_dim = ac;
        n_dim = br;
        fprintf(stderr, "[sgemm] dataset %s / %s: m=%d n=%d k=%d\n",
                fa, fb, m_dim, n_dim, k_dim);
    } else {
        if (matA) { free(matA); matA = nullptr; }
        if (matBT) { free(matBT); matBT = nullptr; }
        synth_fallback();
    }

    long csz = (long)m_dim * n_dim;
    matC = dhir_alloc(csz);
    matC_ref = dhir_alloc(csz);
    dhir_zero(matC, csz);
    dhir_zero(matC_ref, csz);
}

static void bench_call(void) {
    sgemm(m_dim, n_dim, k_dim, ALPHA, BETA,
          MR2(matA, k_dim, m_dim),
          MR2(matBT, k_dim, n_dim),
          MR2(matC, n_dim, m_dim));
}

static void bench_reference(void) {
    ref_sgemm(m_dim, n_dim, k_dim, ALPHA, BETA, matA, matBT, matC_ref);
}

static int bench_check(void) {
    return dhir_compare("C", matC, matC_ref, (long)m_dim * n_dim, DHIR_EPS);
}

static void bench_free(void) {
    if (matA) free(matA);
    if (matBT) free(matBT);
    if (matC) free(matC);
    if (matC_ref) free(matC_ref);
    matA = matBT = matC = matC_ref = nullptr;
}
