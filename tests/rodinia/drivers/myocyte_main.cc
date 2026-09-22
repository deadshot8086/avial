#define DHIR_BENCH_NAME "myocyte"
#include "dhir_bench.h"

extern "C" {
// MLIR Lowered Kernel
void myocyte(int32_t xmax,
             float *, float *, int64_t, int64_t, int64_t, // y (MR1)
             const float *, const float *, int64_t, int64_t, int64_t, // params (MR1)
             float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, // k_stages (MR2: 7 fields)
             float *, float *, int64_t, int64_t, int64_t, // y_prev (MR1)
             float *, float *, int64_t, int64_t, int64_t, // initvalu_temp (MR1)
             float *, float *, int64_t, int64_t, int64_t, // err (MR1)
             float *, float *, int64_t, int64_t, int64_t, // scale (MR1)
             float *, float *, int64_t, int64_t, int64_t, // yy (MR1)
             float *, float *, int64_t, int64_t, int64_t  // stage_buffer (MR1)
);

// Authentic Rodinia 3.1 Reference Implementation
void ref_myocyte(int xmax,
                 float *y,
                 const float *params);
}

static const int num_equations = 91;
static const int num_parameters = 16;
static const int num_stages = 13;
static const int sim_xmax = 100;

static float *y_vec, *y_ref;
static float *params_vec;
static float *k_stages_mat;
static float *y_prev_vec;
static float *initvalu_temp_vec;
static float *err_vec;
static float *scale_vec;
static float *yy_vec;
static float *stage_buffer_vec;

static void read_dataset(const char *filename, float *dest, int count) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", filename);
        exit(1);
    }
    for (int i = 0; i < count; i++) {
        if (fscanf(fp, "%f", &dest[i]) != 1) {
            fprintf(stderr, "Error reading value at index %d from %s\n", i, filename);
            exit(1);
        }
    }
    fclose(fp);
}

static void bench_alloc(void) {
    y_vec = dhir_alloc(num_equations);
    y_ref = dhir_alloc(num_equations);
    params_vec = dhir_alloc(num_parameters);
    k_stages_mat = dhir_alloc((long)num_stages * num_equations);
    y_prev_vec = dhir_alloc(num_equations);
    initvalu_temp_vec = dhir_alloc(num_equations);
    err_vec = dhir_alloc(num_equations);
    scale_vec = dhir_alloc(num_equations);
    yy_vec = dhir_alloc(num_equations);
    stage_buffer_vec = dhir_alloc(num_equations);

    read_dataset("vendor/rodinia/rodinia_3.1/data/myocyte/y.txt", y_vec, num_equations);
    read_dataset("vendor/rodinia/rodinia_3.1/data/myocyte/params.txt", params_vec, num_parameters);

    for (int i = 0; i < num_equations; i++) {
        y_ref[i] = y_vec[i];
    }

    dhir_zero(k_stages_mat, (long)num_stages * num_equations);
    dhir_zero(y_prev_vec, num_equations);
    dhir_zero(initvalu_temp_vec, num_equations);
    dhir_zero(err_vec, num_equations);
    dhir_zero(scale_vec, num_equations);
    dhir_zero(yy_vec, num_equations);
    dhir_zero(stage_buffer_vec, num_equations);
}

static void bench_call(void) {
    myocyte(sim_xmax,
            MR1(y_vec, num_equations),
            MR1(params_vec, num_parameters),
            MR2(k_stages_mat, num_stages, num_equations),
            MR1(y_prev_vec, num_equations),
            MR1(initvalu_temp_vec, num_equations),
            MR1(err_vec, num_equations),
            MR1(scale_vec, num_equations),
            MR1(yy_vec, num_equations),
            MR1(stage_buffer_vec, num_equations));
}

static void bench_reference(void) {
    ref_myocyte(sim_xmax, y_ref, params_vec);
}

static int bench_check(void) {
    int errors = dhir_compare("y", y_vec, y_ref, num_equations, DHIR_EPS);

    return errors;
}

static void bench_free(void) {
    free(y_vec);
    free(y_ref);
    free(params_vec);
    free(k_stages_mat);
    free(y_prev_vec);
    free(initvalu_temp_vec);
    free(err_vec);
    free(scale_vec);
    free(yy_vec);
    free(stage_buffer_vec);
}
