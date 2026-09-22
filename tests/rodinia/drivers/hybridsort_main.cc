#define DHIR_BENCH_NAME "hybridsort"
#include "dhir_bench.h"

extern "C" {
void hybridsort_bucket(float *, float *, int64_t, int64_t, int64_t,
                       int32_t,
                       float *, float *, int64_t, int64_t, int64_t,
                       int32_t,
                       int32_t *, int32_t *, int64_t, int64_t, int64_t,
                       int32_t *, int32_t *, int64_t, int64_t, int64_t);

void ref_hybridsort(const float *input,
                    int size,
                    const float *pivots,
                    int divisions,
                    int *indices,
                    int *bucket_counts);
}

static int data_size = 0;
static const int divisions = 1024;
static const int histosize = 1024;

static float *input_arr = nullptr;
static float *pivots_arr = nullptr;
static int *indices_arr = nullptr, *indices_ref = nullptr;
static int *bucket_counts = nullptr, *bucket_counts_ref = nullptr;

// Given a histogram of the list, figure out suitable pivotpoints that divide
// the list into approximately listsize/divisions elements each.
// Matches exact logic from Rodinia 3.1 cuda/hybridsort/bucketsort.cu
static void calcPivotPoints(const float *histogram, int hist_sz, int listsize,
                            int divs, float min_val, float max_val,
                            float *pivotPoints, float histo_width) {
    float elemsPerSlice = (float)listsize / (float)divs;
    float startsAt = min_val;
    float endsAt = min_val + histo_width;
    float we_need = elemsPerSlice;
    int p_idx = 0;

    float *hist_copy = (float *)malloc(hist_sz * sizeof(float));
    for (int i = 0; i < hist_sz; i++) hist_copy[i] = histogram[i];

    for (int i = 0; i < hist_sz; i++) {
        if (i == hist_sz - 1) {
            if (p_idx < divs) {
                if (hist_copy[i] > 0)
                    pivotPoints[p_idx++] = startsAt + (we_need / hist_copy[i]) * histo_width;
                else
                    pivotPoints[p_idx++] = endsAt;
            }
            break;
        }
        while (hist_copy[i] > we_need) {
            if (p_idx >= divs) break;
            pivotPoints[p_idx++] = startsAt + (we_need / hist_copy[i]) * histo_width;
            startsAt += (we_need / hist_copy[i]) * histo_width;
            hist_copy[i] -= we_need;
            we_need = elemsPerSlice;
        }
        we_need -= hist_copy[i];
        startsAt = endsAt;
        endsAt += histo_width;
    }
    while (p_idx < divs) {
        pivotPoints[p_idx] = (p_idx > 0) ? pivotPoints[p_idx - 1] : max_val;
        p_idx++;
    }
    free(hist_copy);
}

static void bench_alloc(void) {
    const char *candidates[] = {
        "vendor/rodinia/rodinia_3.1/data/hybridsort/500000.txt",
    };

    const char *filepath = getenv("HYBRIDSORT_INPUT");
    FILE *fp = nullptr;

    if (filepath) {
        fp = fopen(filepath, "r");
    }
    if (!fp) {
        for (const char *cand : candidates) {
            fp = fopen(cand, "r");
            if (fp) {
                filepath = cand;
                break;
            }
        }
    }

    if (!fp) {
        fprintf(stderr, "hybridsort: failed to open dataset\n");
        exit(1);
    }

    // Dynamically count total number of floats in the dataset file
    int count = 0;
    float val;
    while (fscanf(fp, "%f", &val) == 1) {
        count++;
    }
    rewind(fp);

    if (count <= 0) {
        fprintf(stderr, "hybridsort: dataset file is empty or invalid\n");
        fclose(fp);
        exit(1);
    }
    data_size = count;

    input_arr = dhir_alloc(data_size);
    pivots_arr = dhir_alloc(divisions);
    indices_arr = dhir_alloc_int(data_size);
    indices_ref = dhir_alloc_int(data_size);
    bucket_counts = dhir_alloc_int(divisions);
    bucket_counts_ref = dhir_alloc_int(divisions);

    for (int i = 0; i < data_size; i++) {
        if (fscanf(fp, "%f", &input_arr[i]) != 1) {
            fprintf(stderr, "hybridsort: error reading float %d from %s\n", i, filepath);
            fclose(fp);
            exit(1);
        }
    }
    fclose(fp);

    // Compute min and max across the entire dataset
    float datamin = input_arr[0], datamax = input_arr[0];
    for (int i = 1; i < data_size; i++) {
        if (input_arr[i] < datamin) datamin = input_arr[i];
        if (input_arr[i] > datamax) datamax = input_arr[i];
    }

    // Compute authentic 1024-bin histogram, matching Rodinia histogram1024GPU
    float *histogram = (float *)calloc(histosize, sizeof(float));
    float range = datamax - datamin;
    if (range <= 0.0f) range = 1.0f;
    for (int i = 0; i < data_size; i++) {
        int b = (int)(((input_arr[i] - datamin) / range) * (float)histosize);
        if (b < 0) b = 0;
        if (b >= histosize) b = histosize - 1;
        histogram[b] += 1.0f;
    }

    // Compute authentic quantile pivot points using Rodinia's calcPivotPoints
    float histo_width = range / (float)histosize;
    calcPivotPoints(histogram, histosize, data_size, divisions,
                    datamin, datamax, pivots_arr, histo_width);
    free(histogram);

    dhir_zero_int(bucket_counts, divisions);
    dhir_zero_int(bucket_counts_ref, divisions);
    dhir_zero_int(indices_arr, data_size);
    dhir_zero_int(indices_ref, data_size);
}

static void bench_call(void) {
    hybridsort_bucket(MR1(input_arr, data_size),
                      data_size,
                      MR1(pivots_arr, divisions),
                      divisions,
                      MR1(indices_arr, data_size),
                      MR1(bucket_counts, divisions));
}

static void bench_reference(void) {
    ref_hybridsort(input_arr, data_size,
                   pivots_arr, divisions,
                   indices_ref, bucket_counts_ref);
}

static int bench_check(void) {
    int errors = dhir_compare_int("indices", indices_arr, indices_ref, data_size);
    errors += dhir_compare_int("bucket_counts", bucket_counts, bucket_counts_ref, divisions);
    return errors;
}

static void bench_free(void) {
    if (input_arr) free(input_arr);
    if (pivots_arr) free(pivots_arr);
    if (indices_arr) free(indices_arr);
    if (indices_ref) free(indices_ref);
    if (bucket_counts) free(bucket_counts);
    if (bucket_counts_ref) free(bucket_counts_ref);
}
