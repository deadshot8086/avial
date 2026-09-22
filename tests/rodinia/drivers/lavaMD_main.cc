#define DHIR_BENCH_NAME "lavaMD"
#include "dhir_bench.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

extern "C" {
void lavaMD_box_interaction(double, int32_t, int32_t,
                            int32_t *, int32_t *, int64_t, int64_t, int64_t,
                            int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
                            double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t,
                            double *, double *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_lavaMD(double alpha, int number_boxes, int max_neighbors,
                const int *box_nn, const int *box_nei,
                const double *rv, const double *qv, double *fv);
}

static int boxes1d = 10;
static int number_boxes = 1000; // boxes1d^3
static const int max_neighbors = 26;
static const int NUMBER_PAR_PER_BOX = 100;
static const double alpha = 0.5;

static int *box_nn;
static int *box_nei;
static double *rv;
static double *qv;
static double *fv, *fv_ref;

static void bench_alloc(void) {
    const char *env_b1d = getenv("LAVAMD_BOXES1D");
    if (env_b1d && atoi(env_b1d) > 0) {
        boxes1d = atoi(env_b1d);
    } else {
        boxes1d = 10;
    }
    number_boxes = boxes1d * boxes1d * boxes1d;

    box_nn = dhir_alloc_int(number_boxes);
    box_nei = dhir_alloc_int(number_boxes * max_neighbors);

    // Box topology in 3D grid matching Rodinia
    int nh = 0;
    for (int i = 0; i < boxes1d; i++) {
        for (int j = 0; j < boxes1d; j++) {
            for (int k = 0; k < boxes1d; k++) {
                box_nn[nh] = 0;
                for (int l = -1; l < 2; l++) {
                    for (int m = -1; m < 2; m++) {
                        for (int n = -1; n < 2; n++) {
                            if (((i+l)>=0 && (j+m)>=0 && (k+n)>=0) &&
                                ((i+l)<boxes1d && (j+m)<boxes1d && (k+n)<boxes1d) &&
                                !(l==0 && m==0 && n==0)) {
                                int nei_idx = (i+l)*boxes1d*boxes1d + (j+m)*boxes1d + (k+n);
                                box_nei[nh * max_neighbors + box_nn[nh]] = nei_idx;
                                box_nn[nh]++;
                            }
                        }
                    }
                }
                nh++;
            }
        }
    }

    long total_particles = (long)number_boxes * NUMBER_PAR_PER_BOX;
    rv = dhir_alloc_double(total_particles * 4);
    qv = dhir_alloc_double(total_particles);
    fv = dhir_alloc_double(total_particles * 4);
    fv_ref = dhir_alloc_double(total_particles * 4);

    // Synthetic data generation matching Rodinia 3.1 multi-stage pipeline
    const char *seed_env = getenv("LAVAMD_SEED");
    int b_seed = (seed_env && atoi(seed_env) > 0) ? atoi(seed_env) : 42;
    srand(b_seed);

    // Stage 1: Distance vectors (v, x, y, z)
    for (long p = 0; p < total_particles; p++) {
        rv[p * 4 + 0] = (rand() % 10 + 1) / 10.0;
        rv[p * 4 + 1] = (rand() % 10 + 1) / 10.0;
        rv[p * 4 + 2] = (rand() % 10 + 1) / 10.0;
        rv[p * 4 + 3] = (rand() % 10 + 1) / 10.0;
    }

    // Stage 2: Charges
    for (long p = 0; p < total_particles; p++) {
        qv[p] = (rand() % 10 + 1) / 10.0;
    }

    // Stage 3: Output forces initialization
    for (long p = 0; p < total_particles; p++) {
        for (int c = 0; c < 4; c++) {
            fv[p * 4 + c] = 0.0;
            fv_ref[p * 4 + c] = 0.0;
        }
    }
}

static void bench_call(void) {
    lavaMD_box_interaction(alpha, number_boxes, max_neighbors,
                           MR1(box_nn, number_boxes),
                           MR2(box_nei, number_boxes, max_neighbors),
                           MR3(rv, number_boxes, NUMBER_PAR_PER_BOX, 4),
                           MR2(qv, number_boxes, NUMBER_PAR_PER_BOX),
                           MR3(fv, number_boxes, NUMBER_PAR_PER_BOX, 4));
}

static void bench_reference(void) {
    ref_lavaMD(alpha, number_boxes, max_neighbors,
               box_nn, box_nei,
               rv, qv, fv_ref);
}

static int bench_check(void) {
    long total_particles = (long)number_boxes * NUMBER_PAR_PER_BOX;
    long total_coords = total_particles * 4;
    return dhir_compare_double("fv", fv, fv_ref, total_coords, 1e-3);
}

static void bench_free(void) {
    if (box_nn) free(box_nn);
    if (box_nei) free(box_nei);
    if (rv) free(rv);
    if (qv) free(qv);
    if (fv) free(fv);
    if (fv_ref) free(fv_ref);
}
