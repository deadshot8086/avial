#define DHIR_BENCH_NAME "backprop"
#include "dhir_bench.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

extern "C" {
void backprop(int32_t, int32_t, int32_t,
              float *, float *, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
              float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_backprop(int in, int hid, int out,
                  float *l1, float *l2, float *l3,
                  const float *target, float *output_delta, float *hidden_delta,
                  float *w_in, float *oldw_in,
                  float *w_hid, float *oldw_hid);
}

static int n1 = 65536;
static const int n2 = 16;
static const int n3 = 1;

static long l1_size;
static long l2_size;
static long l3_size;
static long w_in_size;
static long w_hid_size;

static float *l1;
static float *l2, *l2_ref;
static float *l3, *l3_ref;
static float *target_arr;
static float *output_delta, *output_delta_ref;
static float *hidden_delta, *hidden_delta_ref;
static float *w_in, *w_in_ref;
static float *oldw_in, *oldw_in_ref;
static float *w_hid, *w_hid_ref;
static float *oldw_hid, *oldw_hid_ref;

static void bench_alloc(void) {
    const char *env_n1 = getenv("BACKPROP_SIZE");
    if (!env_n1) env_n1 = getenv("BACKPROP_N1");
    if (env_n1 && atoi(env_n1) > 0) {
        n1 = atoi(env_n1);
    } else {
        n1 = 65536;
    }

    l1_size = n1 + 1;
    l2_size = n2 + 1;
    l3_size = n3 + 1;
    w_in_size = (long)(n1 + 1) * (n2 + 1);
    w_hid_size = (long)(n2 + 1) * (n3 + 1);

    l1 = dhir_alloc(l1_size);
    l2 = dhir_alloc(l2_size);
    l2_ref = dhir_alloc(l2_size);
    l3 = dhir_alloc(l3_size);
    l3_ref = dhir_alloc(l3_size);

    target_arr = dhir_alloc(l3_size);
    output_delta = dhir_alloc(l3_size);
    output_delta_ref = dhir_alloc(l3_size);
    hidden_delta = dhir_alloc(l2_size);
    hidden_delta_ref = dhir_alloc(l2_size);

    w_in = dhir_alloc(w_in_size);
    w_in_ref = dhir_alloc(w_in_size);
    oldw_in = dhir_alloc(w_in_size);
    oldw_in_ref = dhir_alloc(w_in_size);

    w_hid = dhir_alloc(w_hid_size);
    w_hid_ref = dhir_alloc(w_hid_size);
    oldw_hid = dhir_alloc(w_hid_size);
    oldw_hid_ref = dhir_alloc(w_hid_size);

    // Synthetic data generation matching Rodinia 3.1 multi-phase pipeline
    const char *seed_env = getenv("BACKPROP_SEED");
    int seed = (seed_env && atoi(seed_env) > 0) ? atoi(seed_env) : 7;
    srand(seed);

    // Phase 1: bpnn_randomize_weights(input_weights, in, hid)
    for (int i = 0; i <= n1; i++) {
        for (int j = 0; j <= n2; j++) {
            float val = (float)rand() / (float)RAND_MAX;
            w_in[i * (n2 + 1) + j] = val;
            w_in_ref[i * (n2 + 1) + j] = val;
            oldw_in[i * (n2 + 1) + j] = 0.0f;
            oldw_in_ref[i * (n2 + 1) + j] = 0.0f;
        }
    }

    // Phase 2: bpnn_randomize_weights(hidden_weights, hid, out)
    for (int i = 0; i <= n2; i++) {
        for (int j = 0; j <= n3; j++) {
            float val = (float)rand() / (float)RAND_MAX;
            w_hid[i * (n3 + 1) + j] = val;
            w_hid_ref[i * (n3 + 1) + j] = val;
            oldw_hid[i * (n3 + 1) + j] = 0.0f;
            oldw_hid_ref[i * (n3 + 1) + j] = 0.0f;
        }
    }

    // Target
    for (int i = 0; i <= n3; i++) {
        target_arr[i] = 0.1f;
    }

    // Phase 3: load(net) in imagenet.c
    l1[0] = 1.0f;
    for (int i = 1; i <= n1; i++) {
        l1[i] = (float)rand() / (float)RAND_MAX;
    }

    for (int j = 0; j <= n2; j++) {
        l2[j] = 0.0f;
        l2_ref[j] = 0.0f;
        hidden_delta[j] = 0.0f;
        hidden_delta_ref[j] = 0.0f;
    }

    for (int j = 0; j <= n3; j++) {
        l3[j] = 0.0f;
        l3_ref[j] = 0.0f;
        output_delta[j] = 0.0f;
        output_delta_ref[j] = 0.0f;
    }
}

static void bench_call(void) {
    backprop(n1, n2, n3,
             MR1(l1, n1 + 1),
             MR1(l2, n2 + 1),
             MR1(l3, n3 + 1),
             MR1(target_arr, n3 + 1),
             MR1(output_delta, n3 + 1),
             MR1(hidden_delta, n2 + 1),
             MR2(w_in, n1 + 1, n2 + 1),
             MR2(oldw_in, n1 + 1, n2 + 1),
             MR2(w_hid, n2 + 1, n3 + 1),
             MR2(oldw_hid, n2 + 1, n3 + 1));
}

static void bench_reference(void) {
    ref_backprop(n1, n2, n3,
                 l1, l2_ref, l3_ref,
                 target_arr, output_delta_ref, hidden_delta_ref,
                 w_in_ref, oldw_in_ref,
                 w_hid_ref, oldw_hid_ref);
}

static int bench_check(void) {
    int errors = 0;
    errors += dhir_compare("l2", l2, l2_ref, l2_size, DHIR_EPS);
    errors += dhir_compare("l3", l3, l3_ref, l3_size, DHIR_EPS);
    errors += dhir_compare("w_in", w_in, w_in_ref, w_in_size, DHIR_EPS);
    errors += dhir_compare("oldw_in", oldw_in, oldw_in_ref, w_in_size, DHIR_EPS);
    errors += dhir_compare("w_hid", w_hid, w_hid_ref, w_hid_size, DHIR_EPS);
    errors += dhir_compare("oldw_hid", oldw_hid, oldw_hid_ref, w_hid_size, DHIR_EPS);
    return errors;
}

static void bench_free(void) {
    if (l1) free(l1);
    if (l2) free(l2);
    if (l2_ref) free(l2_ref);
    if (l3) free(l3);
    if (l3_ref) free(l3_ref);
    if (target_arr) free(target_arr);
    if (output_delta) free(output_delta);
    if (output_delta_ref) free(output_delta_ref);
    if (hidden_delta) free(hidden_delta);
    if (hidden_delta_ref) free(hidden_delta_ref);
    if (w_in) free(w_in);
    if (w_in_ref) free(w_in_ref);
    if (oldw_in) free(oldw_in);
    if (oldw_in_ref) free(oldw_in_ref);
    if (w_hid) free(w_hid);
    if (w_hid_ref) free(w_hid_ref);
    if (oldw_hid) free(oldw_hid);
    if (oldw_hid_ref) free(oldw_hid_ref);
}
