// MPI driver for the Parboil mri-gridding kernel.

#define DHIR_EPS 2e-3f

#define DHIR_BENCH_NAME "mri_gridding"
#include "dhir_bench.h"


extern "C" {
void mri_gridding(int32_t, int32_t, int32_t, int32_t,
                  float, float, float, float,
                  int32_t, int32_t,
                  /* sample: memref<?x6xf32> */
                  float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                  /* LUT: memref<?xf32> */
                  float *, float *, int64_t, int64_t, int64_t,
                  /* gridData: memref<?x?x?x2xf32> */
                  float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                  int64_t, int64_t, int64_t, int64_t,
                  /* sampleDensity: memref<?x?x?xf32> */
                  float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
                  int64_t, int64_t);

void ref_mri_gridding(int n, int size_x, int size_y, int size_z,
                      float cutoff, float cutoff2, float _1overCutoff2,
                      float beta, int sizeLUT, int useLUT,
                      const float *sample, const float *LUT,
                      float *gridData, float *sampleDensity);
}

#define MG_PI 3.14159265359

static int numSamples = 2655910;
static float kMax[3] = {150.0f, 150.0f, 150.0f};
static int aquisitionMatrixSize[3] = {60, 60, 60};
static int reconstructionMatrixSize[3] = {60, 60, 60};
static int gridSize[3] = {256, 256, 256};
static float oversample = 5.0f;
static float kernelWidth = 5.0f;
static int useLUT = 1;


static int n = 0;

static long gridNumElems = 0;
static int sizeLUT = 0;

static float cutoff = 0.0f, cutoff2 = 0.0f, _1overCutoff2 = 0.0f, beta = 0.0f;

static float *sample = nullptr;      /* [n][6] */
static float *sample_ref = nullptr;
static float *LUT = nullptr;         /* sizeLUT + 1 guard entry */
static float *gridData = nullptr;    /* [size_z][size_y][size_x][2] */
static float *sampleDensity = nullptr;
static float *gridData_ref = nullptr;
static float *sampleDensity_ref = nullptr;

static int loaded_dataset = 0;

static FILE *open_dataset(const char *path, const char *mode) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, mode);
    if (fp) return fp;
    char buf[1024];
    snprintf(buf, sizeof(buf),
             "vendor/parboil/datasets/mri-gridding/small/input/%s", path);
    return fopen(buf, mode);
}

static void read_description_params(const char *uksfile) {
    std::string path(uksfile);
    size_t slash = path.find_last_of('/');
    std::string dir = (slash != std::string::npos) ? path.substr(0, slash) : ".";
    std::string desc = dir + "/DESCRIPTION";
    FILE *f = fopen(desc.c_str(), "r");
    if (!f) {
        std::string p2 = "vendor/parboil/datasets/mri-gridding/small/input/DESCRIPTION";
        f = fopen(p2.c_str(), "r");
    }
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "Parameters:", 11) == 0) {
            char *p = strstr(line, "--");
            if (p) {
                int binsize = 0, flag = 0;
                if (sscanf(p + 2, "%d %d", &binsize, &flag) >= 1) {
                    (void)binsize; (void)flag;
                }
            }
        }
    }
    fclose(f);
}

static int read_params(const char *uksfile) {
    read_description_params(uksfile);
    FILE *f = open_dataset(uksfile, "r");
    if (!f) return 0;
    int ok = 1;
    ok &= (fscanf(f, "aquisition.numsamples=%d\n", &numSamples) == 1);
    ok &= (fscanf(f, "aquisition.kmax=%f %f %f\n",
                  &kMax[0], &kMax[1], &kMax[2]) == 3);
    ok &= (fscanf(f, "aquisition.matrixSize=%d %d %d\n",
                  &aquisitionMatrixSize[0], &aquisitionMatrixSize[1],
                  &aquisitionMatrixSize[2]) == 3);
    ok &= (fscanf(f, "reconstruction.matrixSize=%d %d %d\n",
                  &reconstructionMatrixSize[0], &reconstructionMatrixSize[1],
                  &reconstructionMatrixSize[2]) == 3);
    ok &= (fscanf(f, "gridding.matrixSize=%d %d %d\n",
                  &gridSize[0], &gridSize[1], &gridSize[2]) == 3);
    ok &= (fscanf(f, "gridding.oversampling=%f\n", &oversample) == 1);
    ok &= (fscanf(f, "kernel.width=%f\n", &kernelWidth) == 1);
    ok &= (fscanf(f, "kernel.useLUT=%d\n", &useLUT) == 1);
    fclose(f);
    return ok;
}


static int read_samples(const char *datafile, int want) {
    FILE *f = open_dataset(datafile, "rb");
    if (!f) return 0;
    size_t got = fread(sample, sizeof(float) * 6, (size_t)want, f);
    fclose(f);
    if (got != (size_t)want) {
        fprintf(stderr,
                "%s: short read from %s (%zu/%d samples), using synthetic data\n",
                DHIR_BENCH_NAME, datafile, got, want);
        return 0;
    }

    float kScale[3];
    kScale[0] = (float)(aquisitionMatrixSize[0]) /
                ((float)(reconstructionMatrixSize[0]) * (float)(kMax[0]));
    kScale[1] = (float)(aquisitionMatrixSize[1]) /
                ((float)(reconstructionMatrixSize[1]) * (float)(kMax[1]));
    kScale[2] = (float)(aquisitionMatrixSize[2]) /
                ((float)(reconstructionMatrixSize[2]) * (float)(kMax[2]));

    int size_x = gridSize[0], size_y = gridSize[1], size_z = gridSize[2];

    float ax = (kScale[0] * (size_x - 1)) / 2.0;
    float bx = (float)(size_x - 1) / 2.0;
    float ay = (kScale[1] * (size_y - 1)) / 2.0;
    float by = (float)(size_y - 1) / 2.0;
    float az = (kScale[2] * (size_z - 1)) / 2.0;
    float bz = (float)(size_z - 1) / 2.0;

    for (int i = 0; i < want; i++) {
        sample[6 * i + 2] = floor((sample[6 * i + 2] * ax) + bx);
        sample[6 * i + 3] = floor((sample[6 * i + 3] * ay) + by);
        sample[6 * i + 4] = floor((sample[6 * i + 4] * az) + bz);
    }
    return 1;
}


static void synth_samples(int want) {
    srand(7);
    int size_x = gridSize[0], size_y = gridSize[1], size_z = gridSize[2];
    for (int i = 0; i < want; i++) {
        sample[6 * i + 0] = (float)(rand() % 1000) / 1000.0f;       /* real */
        sample[6 * i + 1] = (float)(rand() % 1000) / 1000.0f;       /* imag */
        sample[6 * i + 2] = (float)(rand() % size_x);               /* kX */
        sample[6 * i + 3] = (float)(rand() % size_y);               /* kY */
        sample[6 * i + 4] = (float)(rand() % size_z);               /* kZ */
        sample[6 * i + 5] = 1.0f + (float)(rand() % 100) / 100.0f;  /* sdc */
    }
}


static float lut_kernel_value(float v) {
    const float z = v * v;
    float num = (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z *
    (z * 0.210580722890567e-22f + 0.380715242345326e-19f) +
     0.479440257548300e-16f) + 0.435125971262668e-13f) +
     0.300931127112960e-10f) + 0.160224679395361e-7f) +
     0.654858370096785e-5f) + 0.202591084143397e-2f) +
     0.463076284721000e0f) + 0.754337328948189e2f) +
     0.830792541809429e4f) + 0.571661130563785e6f) +
     0.216415572361227e8f) + 0.356644482244025e9f) +
     0.144048298227235e10f);
    float den = (z * (z * (z - 0.307646912682801e4f) + 0.347626332405882e7f) -
                 0.144048298227235e10f);
    return -num / den;
}

static void build_lut(void) {
    float cutoff2_lut = (kernelWidth * kernelWidth) / 4.0;
    sizeLUT = (int)(10000 * kernelWidth);
    LUT = dhir_alloc((long)sizeLUT + 1);
    for (int k = 0; k < sizeLUT; ++k) {
        float v = (((float)k) / ((float)sizeLUT)) * cutoff2_lut;
        LUT[k] = lut_kernel_value(beta * sqrt(1.0 - (v / cutoff2_lut)));
    }
    LUT[sizeLUT] = LUT[sizeLUT - 1]; /* guard for the LUT[k0+1] read */
}

static void bench_alloc(void) {
    const char *e;

    const char *uks = getenv("MRI_GRIDDING_INPUT");
    if (!uks) uks = "vendor/parboil/datasets/mri-gridding/small/input/small.uks";

    int have_params = read_params(uks);

    if ((e = getenv("MRI_GRIDDING_N"))) n = atoi(e);
    if (n <= 0 || n > numSamples) n = numSamples;

    if (gridSize[0] < 2) gridSize[0] = 2;
    if (gridSize[1] < 2) gridSize[1] = 2;
    if (gridSize[2] < 2) gridSize[2] = 2;

    gridNumElems = (long)gridSize[0] * gridSize[1] * gridSize[2];

    /* hoisted loop-invariant scalars, exactly as gridding_Gold computes them */
    cutoff = ((float)kernelWidth) / 2.0;
    cutoff2 = cutoff * cutoff;
    _1overCutoff2 = 1 / cutoff2;
    beta = MG_PI * sqrt(4 * kernelWidth * kernelWidth /
                            (oversample * oversample) *
                            (oversample - .5) * (oversample - .5) - .8);

    sample = dhir_alloc((long)n * 6);
    sample_ref = dhir_alloc((long)n * 6);
    gridData = dhir_alloc(gridNumElems * 2);
    sampleDensity = dhir_alloc(gridNumElems);
    gridData_ref = dhir_alloc(gridNumElems * 2);
    sampleDensity_ref = dhir_alloc(gridNumElems);

    char datafile[1200];
    snprintf(datafile, sizeof(datafile), "%s.data", uks);

    loaded_dataset = have_params && read_samples(datafile, n);
    if (!loaded_dataset) synth_samples(n);

    build_lut();

    dhir_zero(gridData, gridNumElems * 2);
    dhir_zero(sampleDensity, gridNumElems);
    dhir_zero(gridData_ref, gridNumElems * 2);
    dhir_zero(sampleDensity_ref, gridNumElems);
    memcpy(sample_ref, sample, (size_t)n * 6 * sizeof(float));

    printf("mri_gridding: %d samples (of %d), grid %dx%dx%d, "
           "kernelWidth=%.1f useLUT=%d sizeLUT=%d, data=%s\n",
           n, numSamples, gridSize[0], gridSize[1], gridSize[2],
           kernelWidth, useLUT, sizeLUT,
           loaded_dataset ? "dataset" : "synthetic");
}

static void bench_call(void) {
    mri_gridding(n, gridSize[0], gridSize[1], gridSize[2],
                 cutoff, cutoff2, _1overCutoff2, beta,
                 sizeLUT, useLUT,
                 MR2(sample, n, 6),
                 MR1(LUT, (long)sizeLUT + 1),
                 MR4(gridData, gridSize[2], gridSize[1], gridSize[0], 2),
                 MR3(sampleDensity, gridSize[2], gridSize[1], gridSize[0]));
}

static void bench_reference(void) {
    ref_mri_gridding(n, gridSize[0], gridSize[1], gridSize[2],
                     cutoff, cutoff2, _1overCutoff2, beta,
                     sizeLUT, useLUT,
                     sample_ref, LUT, gridData_ref, sampleDensity_ref);
}

static int bench_check(void) {
    int errors = 0;
    errors += dhir_compare("gridData", gridData, gridData_ref,
                           gridNumElems * 2, DHIR_EPS);
    errors += dhir_compare("sampleDensity", sampleDensity, sampleDensity_ref,
                           gridNumElems, DHIR_EPS);
    return errors;
}

static void bench_free(void) {
    free(sample);
    free(sample_ref);
    free(LUT);
    free(gridData);
    free(sampleDensity);
    free(gridData_ref);
    free(sampleDensity_ref);
}
