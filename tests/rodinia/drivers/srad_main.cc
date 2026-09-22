#define DHIR_BENCH_NAME "srad"
#include "dhir_bench.h"

extern "C" {
void srad(int32_t Nr, int32_t Nc, int32_t niter, float lambda,
          int32_t r1, int32_t r2, int32_t c1, int32_t c2,
          float *img_alloc, float *img_align, int64_t img_off, int64_t img_s0, int64_t img_s1, int64_t img_st0, int64_t img_st1,
          float *c_alloc, float *c_align, int64_t c_off, int64_t c_s0, int64_t c_s1, int64_t c_st0, int64_t c_st1,
          float *dN_alloc, float *dN_align, int64_t dN_off, int64_t dN_s0, int64_t dN_s1, int64_t dN_st0, int64_t dN_st1,
          float *dS_alloc, float *dS_align, int64_t dS_off, int64_t dS_s0, int64_t dS_s1, int64_t dS_st0, int64_t dS_st1,
          float *dW_alloc, float *dW_align, int64_t dW_off, int64_t dW_s0, int64_t dW_s1, int64_t dW_st0, int64_t dW_st1,
          float *dE_alloc, float *dE_align, int64_t dE_off, int64_t dE_s0, int64_t dE_s1, int64_t dE_st0, int64_t dE_st1);

void ref_srad(int Nr, int Nc, int niter, float lambda,
              int r1, int r2, int c1, int c2,
              float *image,
              float *c,
              float *dN, float *dS, float *dW, float *dE);
}

static int Nr = 0;
static int Nc = 0;
static long Ne = 0;
static int niter = 2;
static float lambda_val = 0.5f;
static int r1 = 0, r2 = 0, c1 = 0, c2 = 0;

static float *image = nullptr, *image_ref = nullptr;
static float *c_buf = nullptr, *dN_buf = nullptr, *dS_buf = nullptr, *dW_buf = nullptr, *dE_buf = nullptr;
static float *c_ref = nullptr, *dN_ref = nullptr, *dS_ref = nullptr, *dW_ref = nullptr, *dE_ref = nullptr;

static void read_pgm_dynamic(const char *filename) {
    FILE *fid = fopen(filename, "r");
    if (!fid) {
        fprintf(stderr, "%s: Cannot open %s for reading\n", DHIR_BENCH_NAME, filename);
        exit(1);
    }
    char tag[16];
    int maxval;
    if (fscanf(fid, "%15s", tag) != 1) {
        fprintf(stderr, "%s: Error reading PGM tag\n", DHIR_BENCH_NAME);
        exit(1);
    }
    if (strcmp(tag, "P2") != 0) {
        fprintf(stderr, "%s: Unsupported PGM format %s (only P2 supported)\n", DHIR_BENCH_NAME, tag);
        exit(1);
    }
    if (fscanf(fid, "%d %d", &Nc, &Nr) != 2) {
        fprintf(stderr, "%s: Error reading PGM dimensions\n", DHIR_BENCH_NAME);
        exit(1);
    }
    if (fscanf(fid, "%d", &maxval) != 1) {
        fprintf(stderr, "%s: Error reading PGM maxval\n", DHIR_BENCH_NAME);
        exit(1);
    }

    Ne = (long)Nr * Nc;
    image = dhir_alloc(Ne);
    image_ref = dhir_alloc(Ne);

    for (int i = 0; i < Nr; i++) {
        for (int j = 0; j < Nc; j++) {
            int val;
            if (fscanf(fid, "%d", &val) != 1) {
                fprintf(stderr, "%s: Error reading PGM pixel data at (%d, %d)\n", DHIR_BENCH_NAME, i, j);
                exit(1);
            }
            float scaled = expf((float)val / 255.0f);
            image[j * Nr + i] = scaled;
            image_ref[j * Nr + i] = scaled;
        }
    }
    fclose(fid);
}

static void bench_alloc(void) {
    const char *input_path = getenv("SRAD_INPUT");
    if (!input_path) {
        input_path = "vendor/rodinia/rodinia_3.1/data/srad/image.pgm";
    }
    const char *niter_env = getenv("SRAD_NITER");
    if (niter_env) niter = atoi(niter_env);
    const char *lambda_env = getenv("SRAD_LAMBDA");
    if (lambda_env) lambda_val = atof(lambda_env);

    /* Dynamically parse dimensions and pixel data directly from input testfile */
    read_pgm_dynamic(input_path);

    /* ROI defaults to entire image matching Rodinia 3.1 srad_v1 */
    r1 = 0;
    r2 = Nr - 1;
    c1 = 0;
    c2 = Nc - 1;

    const char *r1_env = getenv("SRAD_R1"); if (r1_env) r1 = atoi(r1_env);
    const char *r2_env = getenv("SRAD_R2"); if (r2_env) r2 = atoi(r2_env);
    const char *c1_env = getenv("SRAD_C1"); if (c1_env) c1 = atoi(c1_env);
    const char *c2_env = getenv("SRAD_C2"); if (c2_env) c2 = atoi(c2_env);

    c_buf = dhir_alloc(Ne);
    dN_buf = dhir_alloc(Ne);
    dS_buf = dhir_alloc(Ne);
    dW_buf = dhir_alloc(Ne);
    dE_buf = dhir_alloc(Ne);

    c_ref = dhir_alloc(Ne);
    dN_ref = dhir_alloc(Ne);
    dS_ref = dhir_alloc(Ne);
    dW_ref = dhir_alloc(Ne);
    dE_ref = dhir_alloc(Ne);
}

static void bench_call(void) {
    srad(Nr, Nc, niter, lambda_val,
         r1, r2, c1, c2,
         MR2(image, Nc, Nr),
         MR2(c_buf, Nc, Nr),
         MR2(dN_buf, Nc, Nr),
         MR2(dS_buf, Nc, Nr),
         MR2(dW_buf, Nc, Nr),
         MR2(dE_buf, Nc, Nr));
}

static void bench_reference(void) {
    ref_srad(Nr, Nc, niter, lambda_val,
             r1, r2, c1, c2,
             image_ref,
             c_ref,
             dN_ref, dS_ref, dW_ref, dE_ref);
}

static int bench_check(void) {
    return dhir_compare("image", image, image_ref, Ne, DHIR_EPS);
}

static void bench_free(void) {
    free(image); free(image_ref);
    free(c_buf); free(dN_buf); free(dS_buf); free(dW_buf); free(dE_buf);
    free(c_ref); free(dN_ref); free(dS_ref); free(dW_ref); free(dE_ref);
}
