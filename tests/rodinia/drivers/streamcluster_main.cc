#define DHIR_BENCH_NAME "streamcluster"
#include "dhir_bench.h"
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>

extern "C" {
void streamcluster(int32_t, int32_t, int32_t, int32_t,
                   float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                   float *, float *, int64_t, int64_t, int64_t,
                   int32_t *, int32_t *, int64_t, int64_t, int64_t,
                   float *, float *, int64_t, int64_t, int64_t,
                   int32_t *, int32_t *, int64_t, int64_t, int64_t,
                   int32_t *, int32_t *, int64_t, int64_t, int64_t,
                   int32_t *, int32_t *, int64_t, int64_t, int64_t,
                   float *, float *, int64_t, int64_t, int64_t,
                   float *, float *, int64_t, int64_t, int64_t,
                   int32_t *, int32_t *, int64_t, int64_t, int64_t);

void ref_streamcluster(int num, int dim, int kmin, int kmax,
                       float *coord,
                       float *weight,
                       int *assign,
                       float *point_cost,
                       int *is_center,
                       int *center_table,
                       int *switch_membership,
                       float *lower,
                       float *gl_lower,
                       int *feasible);
}

static int num = 65536;
static int dim = 256;
static int kmin = 10;
static int kmax = 20;

static float *coord, *coord_ref;
static float *weight, *weight_ref;
static int *assign, *assign_ref;
static float *cost, *cost_ref;
static int *is_center, *is_center_ref;
static int *center_table, *center_table_ref;
static int *switch_membership, *switch_membership_ref;
static float *lower, *lower_ref;
static float *gl_lower, *gl_lower_ref;
static int *feasible, *feasible_ref;

static unsigned short saved_seed[3];

static void bench_alloc(void) {
    const char *env_n = getenv("STREAMCLUSTER_N");
    if (env_n && atoi(env_n) > 0) num = atoi(env_n);

    const char *env_d = getenv("STREAMCLUSTER_DIM");
    if (env_d && atoi(env_d) > 0) dim = atoi(env_d);

    const char *env_kmin = getenv("STREAMCLUSTER_KMIN");
    if (env_kmin && atoi(env_kmin) > 0) kmin = atoi(env_kmin);

    const char *env_kmax = getenv("STREAMCLUSTER_KMAX");
    if (env_kmax && atoi(env_kmax) > 0) kmax = atoi(env_kmax);

    long total_coords = (long)num * dim;
    coord = dhir_alloc(total_coords);
    coord_ref = dhir_alloc(total_coords);

    weight = dhir_alloc(num);
    weight_ref = dhir_alloc(num);

    assign = dhir_alloc_int(num);
    assign_ref = dhir_alloc_int(num);

    cost = dhir_alloc(num);
    cost_ref = dhir_alloc(num);

    is_center = dhir_alloc_int(num);
    is_center_ref = dhir_alloc_int(num);

    center_table = dhir_alloc_int(num);
    center_table_ref = dhir_alloc_int(num);

    switch_membership = dhir_alloc_int(num);
    switch_membership_ref = dhir_alloc_int(num);

    lower = dhir_alloc(num);
    lower_ref = dhir_alloc(num);

    gl_lower = dhir_alloc(num);
    gl_lower_ref = dhir_alloc(num);

    feasible = dhir_alloc_int(num);
    feasible_ref = dhir_alloc_int(num);

    // Generate synthetic data matching Rodinia 3.1 SimStream (SEED = 1)
    srand48(1);
    for (int i = 0; i < num; i++) {
        for (int d = 0; d < dim; d++) {
            coord[i * dim + d] = (float)lrand48() / (float)INT_MAX;
        }
        weight[i] = 1.0f;
    }
    memcpy(coord_ref, coord, (size_t)total_coords * sizeof(float));
    memcpy(weight_ref, weight, (size_t)num * sizeof(float));

    // Save PRNG state right after synthetic data generation
    unsigned short dummy[3] = {0, 0, 0};
    unsigned short *s = seed48(dummy);
    saved_seed[0] = s[0];
    saved_seed[1] = s[1];
    saved_seed[2] = s[2];
    seed48(saved_seed);
}

static void bench_call(void) {
    seed48(saved_seed);
    streamcluster(num, dim, kmin, kmax,
                  MR2(coord, num, dim),
                  MR1(weight, num),
                  MR1(assign, num),
                  MR1(cost, num),
                  MR1(is_center, num),
                  MR1(center_table, num),
                  MR1(switch_membership, num),
                  MR1(lower, num),
                  MR1(gl_lower, num),
                  MR1(feasible, num));
}

static void bench_reference(void) {
    seed48(saved_seed);
    ref_streamcluster(num, dim, kmin, kmax,
                      coord_ref, weight_ref, assign_ref, cost_ref,
                      is_center_ref, center_table_ref, switch_membership_ref,
                      lower_ref, gl_lower_ref, feasible_ref);
}

static int bench_check(void) {
    int errors = 0;
    errors += dhir_compare_int("is_center", is_center, is_center_ref, num);
    errors += dhir_compare_int("assign", assign, assign_ref, num);
    errors += dhir_compare("weight", weight, weight_ref, num, DHIR_EPS);
    errors += dhir_compare("coord", coord, coord_ref, (long)num * dim, DHIR_EPS);

    return errors;
}

static void bench_free(void) {
    free(coord); free(coord_ref);
    free(weight); free(weight_ref);
    free(assign); free(assign_ref);
    free(cost); free(cost_ref);
    free(is_center); free(is_center_ref);
    free(center_table); free(center_table_ref);
    free(switch_membership); free(switch_membership_ref);
    free(lower); free(lower_ref);
    free(gl_lower); free(gl_lower_ref);
    free(feasible); free(feasible_ref);
}
