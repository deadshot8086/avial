#define DHIR_BENCH_NAME "hotspot"
#include "dhir_bench.h"

extern "C" {
void hotspot(int32_t, int32_t, int32_t,
             float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
             float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
             float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
             float, float, float, float, float);

void ref_hotspot(int row, int col, int num_iterations,
                 const float *power,
                 float *temp,
                 float *result,
                 float Cap_1, float Rx_1, float Ry_1, float Rz_1, float amb_temp);
}

static int grid_rows = 1024;
static int grid_cols = 1024;
static int num_iterations = 2;
static long total_size = 0;

static float *power = nullptr, *temp = nullptr, *temp_ref = nullptr;
static float *result = nullptr, *result_ref = nullptr;
static float Cap_1, Rx_1, Ry_1, Rz_1, amb_temp;

static void read_input(float *vect, int rows, int cols, const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        fprintf(stderr, "%s: Cannot open dataset: %s\n", DHIR_BENCH_NAME, file);
        exit(1);
    }
    char str[256];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fgets(str, sizeof(str), fp) == NULL) {
                fprintf(stderr, "%s: Not enough lines in %s\n", DHIR_BENCH_NAME, file);
                exit(1);
            }
            if (sscanf(str, "%f", &vect[i * cols + j]) != 1) {
                fprintf(stderr, "%s: Invalid format in %s\n", DHIR_BENCH_NAME, file);
                exit(1);
            }
        }
    }
    fclose(fp);
}

static void bench_alloc(void) {
    const char *rows_env = getenv("HOTSPOT_ROWS");
    if (rows_env) grid_rows = atoi(rows_env);
    const char *cols_env = getenv("HOTSPOT_COLS");
    if (cols_env) grid_cols = atoi(cols_env);
    else if (rows_env) grid_cols = grid_rows;

    const char *iter_env = getenv("HOTSPOT_ITERATIONS");
    if (iter_env) num_iterations = atoi(iter_env);

    const char *power_file = getenv("HOTSPOT_POWER");
    if (!power_file) power_file = "vendor/rodinia/rodinia_3.1/data/hotspot/power_1024";

    const char *temp_file = getenv("HOTSPOT_TEMP");
    if (!temp_file) temp_file = "vendor/rodinia/rodinia_3.1/data/hotspot/temp_1024";

    total_size = (long)grid_rows * grid_cols;
    power = dhir_alloc(total_size);
    temp = dhir_alloc(total_size);
    temp_ref = dhir_alloc(total_size);
    result = dhir_alloc(total_size);
    result_ref = dhir_alloc(total_size);

    read_input(temp, grid_rows, grid_cols, temp_file);
    read_input(power, grid_rows, grid_cols, power_file);
    memcpy(temp_ref, temp, (size_t)total_size * sizeof(float));

    dhir_zero(result, total_size);
    dhir_zero(result_ref, total_size);

    /* Chip parameters dynamically computed from runtime dimensions */
    float t_chip = 0.0005f;
    float chip_height = 0.016f;
    float chip_width = 0.016f;
    amb_temp = 80.0f;

    float grid_height = chip_height / grid_rows;
    float grid_width = chip_width / grid_cols;

    float Cap = 0.5f * 1.75e6f * t_chip * grid_width * grid_height;
    float Rx = grid_width / (2.0f * 100.0f * t_chip * grid_height);
    float Ry = grid_height / (2.0f * 100.0f * t_chip * grid_width);
    float Rz = t_chip / (100.0f * grid_height * grid_width);

    float max_slope = 3.0e6f / (0.5f * t_chip * 1.75e6f);
    float step = 0.001f / max_slope / 1000.0f;

    Rx_1 = 1.0f / Rx;
    Ry_1 = 1.0f / Ry;
    Rz_1 = 1.0f / Rz;
    Cap_1 = step / Cap;
}

static void bench_call(void) {
    hotspot(grid_rows, grid_cols, num_iterations,
            MR2(power, grid_rows, grid_cols),
            MR2(temp, grid_rows, grid_cols),
            MR2(result, grid_rows, grid_cols),
            Cap_1, Rx_1, Ry_1, Rz_1, amb_temp);
}

static void bench_reference(void) {
    ref_hotspot(grid_rows, grid_cols, num_iterations,
                power, temp_ref, result_ref,
                Cap_1, Rx_1, Ry_1, Rz_1, amb_temp);
}

static int bench_check(void) {
    return dhir_compare("result", result, result_ref, total_size, DHIR_EPS);
}

static void bench_free(void) {
    free(power);
    free(temp);
    free(temp_ref);
    free(result);
    free(result_ref);
}
