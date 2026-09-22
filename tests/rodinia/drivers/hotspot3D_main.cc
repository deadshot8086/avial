#define DHIR_BENCH_NAME "hotspot3D"
#include "dhir_bench.h"

extern "C" {
void hotspot3D(int32_t, int32_t, int32_t, int32_t,
               float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
               float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
               float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
               float, float, float, float, float, float, float, float, float);

void ref_hotspot3D(int nx, int ny, int nz, int numiter,
                   const float *pIn,
                   float *tIn,
                   float *tOut,
                   float ce, float cw, float cn, float cs,
                   float ct, float cb, float cc,
                   float stepDivCap, float amb_temp);
}

static int numCols = 512;
static int numRows = 512;
static int layers = 8;
static int iterations = 100;
static long total_size = 0;

static float *powerIn = nullptr, *tempIn = nullptr, *tempIn_ref = nullptr;
static float *tempOut = nullptr, *tempOut_ref = nullptr;
static float ce, cw, cn, cs, ct, cb, cc;
static float stepDivCap, amb_temp;

static void readinput(float *vect, int grid_rows, int grid_cols, int nz, const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        fprintf(stderr, "%s: Cannot open dataset: %s\n", DHIR_BENCH_NAME, file);
        exit(1);
    }
    char str[256];
    float val;
    for (int i = 0; i < grid_rows; i++) {
        for (int j = 0; j < grid_cols; j++) {
            for (int k = 0; k < nz; k++) {
                if (fgets(str, sizeof(str), fp) == NULL) {
                    fprintf(stderr, "%s: Error reading file %s\n", DHIR_BENCH_NAME, file);
                    exit(1);
                }
                if (sscanf(str, "%f", &val) != 1) {
                    fprintf(stderr, "%s: Invalid format in %s\n", DHIR_BENCH_NAME, file);
                    exit(1);
                }
                vect[k * grid_rows * grid_cols + i * grid_cols + j] = val;
            }
        }
    }
    fclose(fp);
}

static void bench_alloc(void) {
    const char *cols_env = getenv("HOTSPOT3D_COLS");
    if (cols_env) numCols = atoi(cols_env);
    const char *rows_env = getenv("HOTSPOT3D_ROWS");
    if (rows_env) numRows = atoi(rows_env);
    else numRows = numCols;

    const char *layers_env = getenv("HOTSPOT3D_LAYERS");
    if (layers_env) layers = atoi(layers_env);

    const char *iter_env = getenv("HOTSPOT3D_ITERATIONS");
    if (iter_env) iterations = atoi(iter_env);

    const char *power_file = getenv("HOTSPOT3D_POWER");
    if (!power_file) power_file = "vendor/rodinia/rodinia_3.1/data/hotspot3D/power_512x8";

    const char *temp_file = getenv("HOTSPOT3D_TEMP");
    if (!temp_file) temp_file = "vendor/rodinia/rodinia_3.1/data/hotspot3D/temp_512x8";

    total_size = (long)numCols * numRows * layers;

    powerIn = dhir_alloc(total_size);
    tempIn = dhir_alloc(total_size);
    tempIn_ref = dhir_alloc(total_size);
    tempOut = dhir_alloc(total_size);
    tempOut_ref = dhir_alloc(total_size);

    readinput(powerIn, numRows, numCols, layers, power_file);
    readinput(tempIn, numRows, numCols, layers, temp_file);
    memcpy(tempIn_ref, tempIn, (size_t)total_size * sizeof(float));

    dhir_zero(tempOut, total_size);
    dhir_zero(tempOut_ref, total_size);

    /* Chip parameters dynamically computed from runtime dimensions */
    float t_chip = 0.0005f;
    float chip_height = 0.016f;
    float chip_width = 0.016f;
    amb_temp = 80.0f;

    float dx = chip_height / numRows;
    float dy = chip_width / numCols;
    float dz = t_chip / layers;

    float Cap = 0.5f * 1.75e6f * t_chip * dx * dy;
    float Rx = dy / (2.0f * 100.0f * t_chip * dx);
    float Ry = dx / (2.0f * 100.0f * t_chip * dy);
    float Rz = dz / (100.0f * dx * dy);

    float max_slope = 3.0e6f / (0.5f * t_chip * 1.75e6f);
    float dt = 0.001f / max_slope;

    stepDivCap = dt / Cap;
    ce = stepDivCap / Rx;
    cw = stepDivCap / Rx;
    cn = stepDivCap / Ry;
    cs = stepDivCap / Ry;
    ct = stepDivCap / Rz;
    cb = stepDivCap / Rz;
    cc = 1.0f - (2.0f * ce + 2.0f * cn + 3.0f * ct);
}

static void bench_call(void) {
    hotspot3D(numCols, numRows, layers, iterations,
              MR3(powerIn, layers, numRows, numCols),
              MR3(tempIn, layers, numRows, numCols),
              MR3(tempOut, layers, numRows, numCols),
              ce, cw, cn, cs, ct, cb, cc,
              stepDivCap, amb_temp);
}

static void bench_reference(void) {
    ref_hotspot3D(numCols, numRows, layers, iterations,
                  powerIn, tempIn_ref, tempOut_ref,
                  ce, cw, cn, cs, ct, cb, cc,
                  stepDivCap, amb_temp);
}

static int bench_check(void) {
    return dhir_compare("tempOut", tempOut, tempOut_ref, total_size, DHIR_EPS);
}

static void bench_free(void) {
    free(powerIn);
    free(tempIn);
    free(tempIn_ref);
    free(tempOut);
    free(tempOut_ref);
}
