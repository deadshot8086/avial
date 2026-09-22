#define DHIR_BENCH_NAME "pathfinder"
#include "dhir_bench.h"

extern "C" {
void pathfinder(int32_t rows, int32_t cols,
                int32_t *wall_alloc, int32_t *wall_align, int64_t wall_off,
                int64_t wall_s0, int64_t wall_s1, int64_t wall_st0, int64_t wall_st1,
                int32_t *src_alloc, int32_t *src_align, int64_t src_off,
                int64_t src_s0, int64_t src_st0,
                int32_t *dst_alloc, int32_t *dst_align, int64_t dst_off,
                int64_t dst_s0, int64_t dst_st0);

void ref_pathfinder(int rows, int cols,
                    const int *wall,
                    int *src,
                    int *dst);
}

static int rows = 100;
static int cols = 100000;

static int *wall = nullptr;
static int *wall_ref = nullptr;
static int *src = nullptr;
static int *src_ref = nullptr;
static int *dst = nullptr;
static int *dst_ref = nullptr;

static void init_dims(void) {
    const char *r_env = getenv("PATHFINDER_ROWS");
    if (r_env) rows = atoi(r_env);
    const char *c_env = getenv("PATHFINDER_COLS");
    if (c_env) cols = atoi(c_env);
}

static void bench_alloc(void) {
    init_dims();
    wall = dhir_alloc_int((long)rows * cols);
    wall_ref = dhir_alloc_int((long)rows * cols);
    src = dhir_alloc_int(cols);
    src_ref = dhir_alloc_int(cols);
    dst = dhir_alloc_int(cols);
    dst_ref = dhir_alloc_int(cols);

    /* Deterministic initialization matching Rodinia 3.1 pathfinder benchmark (M_SEED = 9) */
    srand(9);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int val = rand() % 10;
            wall[i * cols + j] = val;
            wall_ref[i * cols + j] = val;
        }
    }
    for (int j = 0; j < cols; j++) {
        // In Rodinia pathfinder: dst = result (wall[0]), src = new int[cols]
        dst[j] = wall[j];
        dst_ref[j] = wall[j];
        src[j] = 0;
        src_ref[j] = 0;
    }
}

static void bench_call(void) {
    pathfinder(rows, cols,
               MR2(wall, rows, cols),
               MR1(src, cols),
               MR1(dst, cols));
}

static void bench_reference(void) {
    ref_pathfinder(rows, cols, wall_ref, src_ref, dst_ref);
}

static int bench_check(void) {
    int err1 = dhir_compare_int("data", wall, wall_ref, (long)rows * cols);
    int err2 = dhir_compare_int("dst", dst, dst_ref, cols);
    return err1 + err2;
}

static void bench_free(void) {
    free(wall);
    free(wall_ref);
    free(src);
    free(src_ref);
    free(dst);
    free(dst_ref);
}
