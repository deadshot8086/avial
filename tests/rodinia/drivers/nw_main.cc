#define DHIR_BENCH_NAME "nw"
#include "dhir_bench.h"

extern "C" {
void needleman_wunsch(int32_t, int32_t,
                      int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
                      int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
                      int32_t);

void ref_nw(int max_rows, int max_cols,
            int *input_itemsets,
            const int *reference,
            int penalty);
}

static int dim = 2048;
static int max_rows;
static int max_cols;
static int penalty = 10;
static long total_elements;

static int *input_itemsets, *input_ref;
static int *reference_matrix;

static const int blosum62[24][24] = {
{ 4, -1, -2, -2,  0, -1, -1,  0, -2, -1, -1, -1, -1, -2, -1,  1,  0, -3, -2,  0, -2, -1,  0, -4},
{-1,  5,  0, -2, -3,  1,  0, -2,  0, -3, -2,  2, -1, -3, -2, -1, -1, -3, -2, -3, -1,  0, -1, -4},
{-2,  0,  6,  1, -3,  0,  0,  0,  1, -3, -3,  0, -2, -3, -2,  1,  0, -4, -2, -3,  3,  0, -1, -4},
{-2, -2,  1,  6, -3,  0,  2, -1, -1, -3, -4, -1, -3, -3, -1,  0, -1, -4, -3, -3,  4,  1, -1, -4},
{ 0, -3, -3, -3,  9, -3, -4, -3, -3, -1, -1, -3, -1, -2, -3, -1, -1, -2, -2, -1, -3, -3, -2, -4},
{-1,  1,  0,  0, -3,  5,  2, -2,  0, -3, -2,  1,  0, -3, -1,  0, -1, -2, -1, -2,  0,  3, -1, -4},
{-1,  0,  0,  2, -4,  2,  5, -2,  0, -3, -3,  1, -2, -3, -1,  0, -1, -3, -2, -2,  1,  4, -1, -4},
{ 0, -2,  0, -1, -3, -2, -2,  6, -2, -4, -4, -2, -3, -3, -2,  0, -2, -2, -3, -3, -1, -2, -1, -4},
{-2,  0,  1, -1, -3,  0,  0, -2,  8, -3, -3, -1, -2, -1, -2, -1, -2, -2,  2, -3,  0,  0, -1, -4},
{-1, -3, -3, -3, -1, -3, -3, -4, -3,  4,  2, -3,  1,  0, -3, -2, -1, -3, -1,  3, -3, -3, -1, -4},
{-1, -2, -3, -4, -1, -2, -3, -4, -3,  2,  4, -2,  2,  0, -3, -2, -1, -2, -1,  1, -4, -3, -1, -4},
{-1,  2,  0, -1, -3,  1,  1, -2, -1, -3, -2,  5, -1, -3, -1,  0, -1, -3, -2, -2,  0,  1, -1, -4},
{-1, -1, -2, -3, -1,  0, -2, -3, -2,  1,  2, -1,  5,  0, -2, -1, -1, -1, -1,  1, -3, -1, -1, -4},
{-2, -3, -3, -3, -2, -3, -3, -3, -1,  0,  0, -3,  0,  6, -4, -2, -2,  1,  3, -1, -3, -3, -1, -4},
{-1, -2, -2, -1, -3, -1, -1, -2, -2, -3, -3, -1, -2, -4,  7, -1, -1, -4, -3, -2, -2, -1, -2, -4},
{ 1, -1,  1,  0, -1,  0,  0,  0, -1, -2, -2,  0, -1, -2, -1,  4,  1, -3, -2, -2,  0,  0,  0, -4},
{ 0, -1,  0, -1, -1, -1, -1, -2, -2, -1, -1, -1, -1, -2, -1,  1,  5, -2, -2,  0, -1, -1,  0, -4},
{-3, -3, -4, -4, -2, -2, -3, -2, -2, -3, -2, -3, -1,  1, -4, -3, -2, 11,  2, -3, -4, -3, -2, -4},
{-2, -2, -2, -3, -2, -1, -2, -3,  2, -1, -1, -2, -1,  3, -3, -2, -2,  2,  7, -1, -3, -2, -1, -4},
{ 0, -3, -3, -3, -1, -2, -2, -3, -3,  3,  1, -2,  1, -1, -2, -2,  0, -3, -1,  4, -3, -2, -1, -4},
{-2, -1,  3,  4, -3,  0,  1, -1,  0, -3, -4,  0, -3, -3, -2,  0, -1, -4, -3, -3,  4,  1, -1, -4},
{-1,  0,  0,  1, -3,  3,  4, -2,  0, -3, -3,  1, -1, -3, -1,  0, -1, -3, -2, -2,  1,  4, -1, -4},
{ 0, -1, -1, -1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2,  0,  0, -2, -1, -1, -1, -1, -1, -4},
{-4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4,  1}
};

static void bench_alloc(void) {
    const char *dim_env = getenv("NW_DIM");
    if (dim_env) {
        dim = atoi(dim_env);
    }
    const char *pen_env = getenv("NW_PENALTY");
    if (pen_env) {
        penalty = atoi(pen_env);
    }

    max_rows = dim + 1;
    max_cols = dim + 1;
    total_elements = (long)max_rows * max_cols;

    input_itemsets = dhir_alloc_int(total_elements);
    input_ref = dhir_alloc_int(total_elements);
    reference_matrix = dhir_alloc_int(total_elements);

    srand(7);
    for (long k = 0; k < total_elements; k++) {
        input_itemsets[k] = 0;
        reference_matrix[k] = 0;
    }

    // Sequence 1 along column 0
    for (int i = 1; i < max_rows; i++) {
        input_itemsets[i * max_cols] = rand() % 10 + 1;
    }
    // Sequence 2 along row 0
    for (int j = 1; j < max_cols; j++) {
        input_itemsets[j] = rand() % 10 + 1;
    }

    // Reference matrix lookup from blosum62
    for (int i = 1; i < max_rows; i++) {
        for (int j = 1; j < max_cols; j++) {
            reference_matrix[i * max_cols + j] = blosum62[input_itemsets[i * max_cols]][input_itemsets[j]];
        }
    }

    // Boundary penalties
    for (int i = 1; i < max_rows; i++) {
        input_itemsets[i * max_cols] = -i * penalty;
    }
    for (int j = 1; j < max_cols; j++) {
        input_itemsets[j] = -j * penalty;
    }

    // Copy initial state to input_ref
    for (long k = 0; k < total_elements; k++) {
        input_ref[k] = input_itemsets[k];
    }
}

static void bench_call(void) {
    needleman_wunsch(max_rows, max_cols,
                     MR2(input_itemsets, max_rows, max_cols),
                     MR2(reference_matrix, max_rows, max_cols),
                     penalty);
}

static void bench_reference(void) {
    ref_nw(max_rows, max_cols, input_ref, reference_matrix, penalty);
}

static int bench_check(void) {
    return dhir_compare_int("input_itemsets", input_itemsets, input_ref, total_elements);
}

static void bench_free(void) {
    free(input_itemsets);
    free(input_ref);
    free(reference_matrix);
}
