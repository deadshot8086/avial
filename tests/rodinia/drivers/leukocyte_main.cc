#define DHIR_BENCH_NAME "leukocyte"
#include "dhir_bench.h"

extern "C" {
int load_avi_gradients(const char *filename, int frame_num,
                       int *out_width, int *out_height,
                       double **out_grad_x, double **out_grad_y);

void leukocyte(int32_t, int32_t, int32_t,
               double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t,
               double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t,
               int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
               int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
               double *, double *, int64_t, int64_t, int64_t,
               double *, double *, int64_t, int64_t, int64_t,
               int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
               double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t,
               double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_leukocyte(int width, int height, int MaxR,
                   const double *grad_x,
                   const double *grad_y,
                   const int *tX,
                   const int *tY,
                   const double *cos_angle,
                   const double *sin_angle,
                   const int *strel,
                   double *Grad,
                   double *gicov,
                   double *dilated);
}

static const int NCIRCLES = 7;
static const int NPOINTS = 150;
static const int RADIUS = 10;
static const int MIN_RAD = 8;
static const int MAX_RAD = 20;
static const int MaxR = 22;

static const int STREL_M = 25;
static const int STREL_N = 25;
static const int STREL_RADIUS = 12;

static int width = 0;
static int height = 0;
static long num_pixels = 0;

static double *grad_x = nullptr, *grad_y = nullptr;
static int *tX = nullptr, *tY = nullptr;
static double *cos_angle = nullptr, *sin_angle = nullptr, *Grad = nullptr;
static int *strel = nullptr;
static double *gicov = nullptr, *gicov_ref = nullptr;
static double *dilated = nullptr, *dilated_ref = nullptr;

static void bench_alloc(void) {
    const char *candidates[] = {
        "vendor/rodinia/rodinia_3.1/data/leukocyte/testfile.avi",
    };
    const char *avi_path = nullptr;
    for (const char *path : candidates) {
        FILE *f = fopen(path, "rb");
        if (f) {
            fclose(f);
            avi_path = path;
            break;
        }
    }
    if (!avi_path) {
        fprintf(stderr, "Error: testfile.avi not found\n");
        exit(1);
    }

    if (load_avi_gradients(avi_path, 0, &width, &height, &grad_x, &grad_y) != 0) {
        fprintf(stderr, "Error: failed to load gradients from %s\n", avi_path);
        exit(1);
    }

    num_pixels = (long)width * height;
    tX = dhir_alloc_int(NCIRCLES * NPOINTS);
    tY = dhir_alloc_int(NCIRCLES * NPOINTS);
    cos_angle = dhir_alloc_double(NPOINTS);
    sin_angle = dhir_alloc_double(NPOINTS);
    Grad = dhir_alloc_double(NPOINTS);

    strel = dhir_alloc_int(STREL_M * STREL_N);
    for (int el_i = 0; el_i < STREL_M; el_i++) {
        for (int el_j = 0; el_j < STREL_N; el_j++) {
            float dist = sqrtf((float)((el_i - STREL_RADIUS) * (el_i - STREL_RADIUS) +
                                       (el_j - STREL_RADIUS) * (el_j - STREL_RADIUS)));
            strel[el_i * STREL_N + el_j] = (dist <= (float)STREL_RADIUS) ? 1 : 0;
        }
    }

    gicov = dhir_alloc_double(num_pixels);
    gicov_ref = dhir_alloc_double(num_pixels);
    dilated = dhir_alloc_double(num_pixels);
    dilated_ref = dhir_alloc_double(num_pixels);

    dhir_zero_double(gicov, num_pixels);
    dhir_zero_double(gicov_ref, num_pixels);
    dhir_zero_double(dilated, num_pixels);
    dhir_zero_double(dilated_ref, num_pixels);
    dhir_zero_double(Grad, NPOINTS);

    const double PI = 3.14159;
    for (int n = 0; n < NPOINTS; n++) {
        double theta = (((double)n) * 2.0 * PI) / ((double)NPOINTS);
        cos_angle[n] = cos(theta);
        sin_angle[n] = sin(theta);
    }

    for (int k = 0; k < NCIRCLES; k++) {
        double rad = (double)(MIN_RAD + 2 * k);
        for (int n = 0; n < NPOINTS; n++) {
            tX[k * NPOINTS + n] = (int)(cos_angle[n] * rad);
            tY[k * NPOINTS + n] = (int)(sin_angle[n] * rad);
        }
    }
}

static void bench_call(void) {
    leukocyte(width, height, MaxR,
              MR2(grad_x, height, width),
              MR2(grad_y, height, width),
              MR2(tX, NCIRCLES, NPOINTS),
              MR2(tY, NCIRCLES, NPOINTS),
              MR1(cos_angle, NPOINTS),
              MR1(sin_angle, NPOINTS),
              MR2(strel, STREL_M, STREL_N),
              MR2(gicov, height, width),
              MR2(dilated, height, width));
}

static void bench_reference(void) {
    ref_leukocyte(width, height, MaxR,
                  grad_x, grad_y,
                  tX, tY,
                  cos_angle, sin_angle,
                  strel,
                  Grad,
                  gicov_ref,
                  dilated_ref);
}

static int bench_check(void) {
    int errors = 0;
    errors += dhir_compare_double("gicov", gicov, gicov_ref, num_pixels, DHIR_EPS);
    errors += dhir_compare_double("dilated", dilated, dilated_ref, num_pixels, DHIR_EPS);

    return errors;
}

static void bench_free(void) {
    if (grad_x) free(grad_x);
    if (grad_y) free(grad_y);
    if (tX) free(tX);
    if (tY) free(tY);
    if (cos_angle) free(cos_angle);
    if (sin_angle) free(sin_angle);
    if (strel) free(strel);
    if (Grad) free(Grad);
    if (gicov) free(gicov);
    if (gicov_ref) free(gicov_ref);
    if (dilated) free(dilated);
    if (dilated_ref) free(dilated_ref);
}
