// MPI driver for the extracted Parboil mri_q kernel.


#define DHIR_BENCH_NAME "mri_q"
#include "dhir_bench.h"


extern "C" {
void mri_q(int32_t, int32_t,
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, // kVals
           float *, float *, int64_t, int64_t, int64_t,   // phiR
           float *, float *, int64_t, int64_t, int64_t,   // phiI
           float *, float *, int64_t, int64_t, int64_t,   // x
           float *, float *, int64_t, int64_t, int64_t,   // y
           float *, float *, int64_t, int64_t, int64_t,   // z
           float *, float *, int64_t, int64_t, int64_t,   // Qr
           float *, float *, int64_t, int64_t, int64_t);  // Qi

void ref_mri_q(int numK, int numX, float *kVals,
               const float *phiR, const float *phiI,
               const float *x, const float *y, const float *z,
               float *Qr, float *Qi);
}

#include <string>

static int numK = 0;   // k-space samples
static int numX = 0;   // output pixels

static float *kx = nullptr, *ky = nullptr, *kz = nullptr;
static float *xs = nullptr, *ys = nullptr, *zs = nullptr;
static float *phiR = nullptr, *phiI = nullptr;

static float *kVals = nullptr;      // [numK][4], kernel-side
static float *kVals_ref = nullptr;  // [numK][4], reference-side
static float *Qr = nullptr, *Qi = nullptr;
static float *Qr_ref = nullptr, *Qi_ref = nullptr;


static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "rb");
    if (fp) return fp;

    std::string p1 = std::string("vendor/parboil/datasets/mri-q/small/input/") + path;
    fp = fopen(p1.c_str(), "rb");
    return fp;
}


static bool read_dataset(const char *path) {
    FILE *fid = open_resolved(path);
    if (!fid) return false;

    int nk = 0, nx = 0;
    if (fread(&nk, sizeof(int), 1, fid) != 1 ||
        fread(&nx, sizeof(int), 1, fid) != 1 || nk <= 0 || nx <= 0) {
        fclose(fid);
        return false;
    }

    float *bufs[8];
    long lens[8] = {nk, nk, nk, nx, nx, nx, nk, nk};
    for (int i = 0; i < 8; ++i) bufs[i] = dhir_alloc(lens[i]);

    bool ok = true;
    for (int i = 0; i < 8 && ok; ++i)
        ok = (fread(bufs[i], sizeof(float), (size_t)lens[i], fid) == (size_t)lens[i]);
    fclose(fid);

    if (!ok) {
        for (int i = 0; i < 8; ++i) free(bufs[i]);
        fprintf(stderr, "[mri_q] short read from %s, falling back to synthetic\n", path);
        return false;
    }

    numK = nk;
    numX = nx;
    kx = bufs[0]; ky = bufs[1]; kz = bufs[2];
    xs = bufs[3]; ys = bufs[4]; zs = bufs[5];
    phiR = bufs[6]; phiI = bufs[7];
    return true;
}

static void synth_fill(void) {
    if (numK <= 0) numK = 3072;
    if (numX <= 0) numX = 32768;

    kx = dhir_alloc(numK); ky = dhir_alloc(numK); kz = dhir_alloc(numK);
    xs = dhir_alloc(numX); ys = dhir_alloc(numX); zs = dhir_alloc(numX);
    phiR = dhir_alloc(numK); phiI = dhir_alloc(numK);

    srand(7);
    for (int k = 0; k < numK; ++k) {
        kx[k] = (float)((rand() % 2000) - 1000) / 2000.0f;
        ky[k] = (float)((rand() % 2000) - 1000) / 2000.0f;
        kz[k] = (float)((rand() % 2000) - 1000) / 2000.0f;
        phiR[k] = (float)((rand() % 2000) - 1000) / 1000.0f;
        phiI[k] = (float)((rand() % 2000) - 1000) / 1000.0f;
    }
    for (int i = 0; i < numX; ++i) {
        xs[i] = (float)((rand() % 2000) - 1000) / 2000.0f;
        ys[i] = (float)((rand() % 2000) - 1000) / 2000.0f;
        zs[i] = (float)((rand() % 2000) - 1000) / 2000.0f;
    }
}

static void bench_alloc(void) {
    const char *input = getenv("MRI_Q_INPUT");
    if (!input) input = "vendor/parboil/datasets/mri-q/small/input/32_32_32_dataset.bin";

    bool real_data = read_dataset(input);

    if (!real_data) {
        if (const char *e = getenv("MRI_Q_NUMK")) { int v = atoi(e); if (v > 0) numK = v; }
        if (const char *e = getenv("MRI_Q_NUMX")) { int v = atoi(e); if (v > 0) numX = v; }
        fprintf(stderr, "[mri_q] dataset %s unavailable, using synthetic data "
                        "(numK=%d numX=%d, srand(7))\n",
                input, numK > 0 ? numK : 3072, numX > 0 ? numX : 32768);
        synth_fill();
    } else if (const char *e = getenv("MRI_Q_NUMK")) {
        /* main.c's optional argv[1]: numK = MIN(inputK, original_numK) */
        int v = atoi(e);
        if (v > 0 && v < numK) numK = v;
    }

    kVals = dhir_alloc((long)numK * 4);
    kVals_ref = dhir_alloc((long)numK * 4);
    Qr = dhir_alloc(numX);
    Qi = dhir_alloc(numX);
    Qr_ref = dhir_alloc(numX);
    Qi_ref = dhir_alloc(numX);

    for (int k = 0; k < numK; ++k) {
        kVals[k * 4 + 0] = kx[k];
        kVals[k * 4 + 1] = ky[k];
        kVals[k * 4 + 2] = kz[k];
        kVals[k * 4 + 3] = 0.0f;
    }
    memcpy(kVals_ref, kVals, (size_t)numK * 4 * sizeof(float));

    dhir_zero(Qr, numX);
    dhir_zero(Qi, numX);
    dhir_zero(Qr_ref, numX);
    dhir_zero(Qi_ref, numX);

    printf("[mri_q] %s numK=%d numX=%d\n", real_data ? "dataset" : "synthetic",
           numK, numX);
}

static void bench_call(void) {
    mri_q(numK, numX,
          MR2(kVals, numK, 4),
          MR1(phiR, numK),
          MR1(phiI, numK),
          MR1(xs, numX),
          MR1(ys, numX),
          MR1(zs, numX),
          MR1(Qr, numX),
          MR1(Qi, numX));
}

static void bench_reference(void) {
    ref_mri_q(numK, numX, kVals_ref, phiR, phiI, xs, ys, zs, Qr_ref, Qi_ref);
}

static int bench_check(void) {
    int errors = dhir_compare("Qr", Qr, Qr_ref, numX, DHIR_EPS);
    errors += dhir_compare("Qi", Qi, Qi_ref, numX, DHIR_EPS);
    errors += dhir_compare("kVals", kVals, kVals_ref, (long)numK * 4, DHIR_EPS);
    return errors;
}

static void bench_free(void) {
    free(kx); free(ky); free(kz);
    free(xs); free(ys); free(zs);
    free(phiR); free(phiI);
    free(kVals); free(kVals_ref);
    free(Qr); free(Qi); free(Qr_ref); free(Qi_ref);
    kx = ky = kz = xs = ys = zs = phiR = phiI = nullptr;
    kVals = kVals_ref = Qr = Qi = Qr_ref = Qi_ref = nullptr;
}
