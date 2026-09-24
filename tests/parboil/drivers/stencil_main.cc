#define DHIR_BENCH_NAME "stencil"
#include "dhir_bench.h"

extern "C" {
void stencil(int32_t, int32_t, int32_t, int32_t,
             float *, float *, int64_t, int64_t, int64_t, int64_t,
             int64_t, int64_t, int64_t,
             float *, float *, int64_t, int64_t, int64_t, int64_t,
             int64_t, int64_t, int64_t,
             float, float);

void ref_stencil(int nx, int ny, int nz, int iterations,
                 float *A0, float *Anext,
                 float c0, float c1);
}


static int nx = 0;
static int ny = 0;
static int nz = 0;
static int iterations = 0;
static long total_size = 0;


static float c0 = 1.0f / 6.0f;
static float c1 = 1.0f / 6.0f / 6.0f;

static float *A0 = nullptr, *Anext = nullptr;
static float *A0_ref = nullptr, *Anext_ref = nullptr;
static float *out = nullptr, *out_ref = nullptr;

static FILE *open_dataset(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "rb");
    if (fp) return fp;
    char buf[1024];
    snprintf(buf, sizeof(buf), "vendor/parboil/datasets/stencil/default/input/%s", path);
    fp = fopen(buf, "rb");
    return fp;
}

static bool read_description_params(const char *dataset_path, char *buf, size_t maxlen) {
    if (!dataset_path) return false;
    char desc_path[1024];
    const char *last_slash = strrchr(dataset_path, '/');
    if (last_slash) {
        size_t dirlen = last_slash - dataset_path;
        if (dirlen >= sizeof(desc_path) - 16) return false;
        strncpy(desc_path, dataset_path, dirlen);
        desc_path[dirlen] = '\0';
        strcat(desc_path, "/DESCRIPTION");
    } else {
        snprintf(desc_path, sizeof(desc_path), "DESCRIPTION");
    }

    FILE *fp = fopen(desc_path, "r");
    if (!fp) {
        snprintf(desc_path, sizeof(desc_path), "vendor/parboil/datasets/stencil/default/input/DESCRIPTION");
        fp = fopen(desc_path, "r");
    }
    if (!fp) return false;

    char line[512];
    bool found = false;
    while (fgets(line, sizeof(line), fp)) {
        char *p = strstr(line, "Parameters:");
        if (p) {
            p += strlen("Parameters:");
            while (*p == ' ' || *p == '\t' || *p == '-') {
                p++;
            }
            strncpy(buf, p, maxlen - 1);
            buf[maxlen - 1] = '\0';
            found = true;
            break;
        }
    }
    fclose(fp);
    return found;
}

static void synth_fill(float *p, long n) {
    srand(7);
    for (long i = 0; i < n; ++i)
        p[i] = (float)(rand() % 1000) / 1000.0f;
}

static void bench_alloc(void) {
    const char *input = getenv("STENCIL_INPUT");
    if (!input) input = "vendor/parboil/datasets/stencil/default/input/512x512x64x100.bin";

    char params[256] = {0};
    if (read_description_params(input, params, sizeof(params))) {
        sscanf(params, "%d %d %d %d", &nx, &ny, &nz, &iterations);
    }

    if (nx <= 0 || ny <= 0 || nz <= 0) {
        const char *base = strrchr(input, '/');
        base = base ? base + 1 : input;
        sscanf(base, "%dx%dx%d", &nx, &ny, &nz);
    }
    if (iterations <= 0) iterations = 100;

    const char *e;
    if ((e = getenv("STENCIL_NX"))) nx = atoi(e);
    if ((e = getenv("STENCIL_NY"))) ny = atoi(e);
    if ((e = getenv("STENCIL_NZ"))) nz = atoi(e);
    if ((e = getenv("STENCIL_ITERATIONS"))) iterations = atoi(e);
    if (nx < 3) nx = 128;
    if (ny < 3) ny = 128;
    if (nz < 3) nz = 32;
    if (iterations < 1) iterations = 100;

    total_size = (long)nx * ny * nz;

    A0 = dhir_alloc(total_size);
    Anext = dhir_alloc(total_size);
    A0_ref = dhir_alloc(total_size);
    Anext_ref = dhir_alloc(total_size);

    int loaded = 0;
    FILE *fp = open_dataset(input);
    if (fp) {
        size_t got = fread(A0, sizeof(float), (size_t)total_size, fp);
        fclose(fp);
        if (got == (size_t)total_size) loaded = 1;
        else fprintf(stderr, "%s: short read from %s (%zu/%ld floats), using synthetic data\n",
                     DHIR_BENCH_NAME, input, got, total_size);
    }
    if (!loaded) synth_fill(A0, total_size);

    printf("stencil: %dx%dx%d, %d iterations, data=%s\n",
           nx, ny, nz, iterations, loaded ? "dataset" : "synthetic");

    memcpy(Anext, A0, (size_t)total_size * sizeof(float));
    memcpy(A0_ref, A0, (size_t)total_size * sizeof(float));
    memcpy(Anext_ref, A0, (size_t)total_size * sizeof(float));
}

static void bench_call(void) {
    stencil(nx, ny, nz, iterations,
            MR3(A0, nz, ny, nx),
            MR3(Anext, nz, ny, nx),
            c0, c1);
}

static void bench_reference(void) {
    ref_stencil(nx, ny, nz, iterations, A0_ref, Anext_ref, c0, c1);
}

static int bench_check(void) {
    float *out = (iterations % 2 == 1) ? Anext : A0;
    float *out_ref = (iterations % 2 == 1) ? Anext_ref : A0_ref;
    return dhir_compare("Anext", out, out_ref, total_size, DHIR_EPS);
}

static void bench_free(void) {
    free(A0);
    free(Anext);
    free(A0_ref);
    free(Anext_ref);
}
