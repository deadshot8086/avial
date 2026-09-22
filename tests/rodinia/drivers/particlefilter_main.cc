#define DHIR_BENCH_NAME "particlefilter"
#include "dhir_bench.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

extern "C" {
void particle_filter(
    int32_t Nparticles, int32_t countOnes, int32_t max_size,
    int32_t IszX, int32_t IszY, int32_t Nfr,
    int32_t *, int32_t *, int64_t, int64_t, int64_t, // I
    int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t, // objxy
    int32_t *, int32_t *, int64_t, int64_t, int64_t, // seed
    double *, double *, int64_t, int64_t, int64_t, // arrayX
    double *, double *, int64_t, int64_t, int64_t, // arrayY
    double *, double *, int64_t, int64_t, int64_t, // weights
    double *, double *, int64_t, int64_t, int64_t, // likelihood
    double *, double *, int64_t, int64_t, int64_t, // CDF
    double *, double *, int64_t, int64_t, int64_t, // u
    double *, double *, int64_t, int64_t, int64_t, // xj
    double *, double *, int64_t, int64_t, int64_t, // yj
    double *, double *, int64_t, int64_t, int64_t, // out_xe
    double *, double *, int64_t, int64_t, int64_t  // out_ye
);

void ref_particle_filter(
    int Nparticles, int countOnes, int max_size,
    int IszX, int IszY, int Nfr,
    const int *I,
    const int *objxy,
    int *seed,
    double *arrayX,
    double *arrayY,
    double *weights,
    double *likelihood,
    double *CDF,
    double *u,
    double *xj,
    double *yj,
    double *out_xe,
    double *out_ye
);

void pf_strel_disk(int *disk, int radius);
void pf_getneighbors(int *se, int numOnes, int *neighbors, int radius);
void pf_video_sequence(int *I, int IszX, int IszY, int Nfr, int *seed);
}

// Authentic Rodinia 3.1 default parameters
static int Nparticles = 10000;
static const int IszX = 128;
static const int IszY = 128;
static const int Nfr = 10;
static int max_size = IszX * IszY * Nfr;

static const int radius = 5;
static const int diameter = radius * 2 - 1;
static int countOnes = 69;

// Inputs
static int *I_image = nullptr;
static int *objxy = nullptr;

// MLIR State Arrays
static int *seed = nullptr;
static double *arrayX = nullptr;
static double *arrayY = nullptr;
static double *weights = nullptr;
static double *likelihood = nullptr;
static double *CDF = nullptr;
static double *u = nullptr;
static double *xj = nullptr;
static double *yj = nullptr;
static double *out_xe = nullptr;
static double *out_ye = nullptr;

// Reference State Arrays
static int *seed_ref = nullptr;
static double *arrayX_ref = nullptr;
static double *arrayY_ref = nullptr;
static double *weights_ref = nullptr;
static double *likelihood_ref = nullptr;
static double *CDF_ref = nullptr;
static double *u_ref = nullptr;
static double *xj_ref = nullptr;
static double *yj_ref = nullptr;
static double *out_xe_ref = nullptr;
static double *out_ye_ref = nullptr;

static inline double roundDouble(double value) {
    int newValue = (int)(value);
    if (value - newValue < 0.5)
        return (double)newValue;
    else
        return (double)(newValue++);
}

static void bench_alloc(void) {
    const char *np_env = getenv("PARTICLEFILTER_NP");
    if (np_env) {
        int val = atoi(np_env);
        if (val > 0) Nparticles = val;
    }
    max_size = IszX * IszY * Nfr;

    // 1. Build circular disk structuring element (radius = 5)
    int *disk = (int *)malloc(sizeof(int) * diameter * diameter);
    pf_strel_disk(disk, radius);
    countOnes = 0;
    for (int i = 0; i < diameter * diameter; i++) {
        if (disk[i]) countOnes++;
    }
    objxy = (int *)malloc(sizeof(int) * countOnes * 2);
    pf_getneighbors(disk, countOnes, objxy, radius);
    free(disk);

    // 2. Allocate seeds deterministically (matching Rodinia 3.1)
    seed = dhir_alloc_int(Nparticles);
    seed_ref = dhir_alloc_int(Nparticles);
    const char *s_env = getenv("PARTICLEFILTER_SEED");
    long b_seed = s_env ? atol(s_env) : 42;
    for (int i = 0; i < Nparticles; i++) {
        seed[i] = (int)(b_seed * (i + 1));
    }

    // 3. Synthesize authentic Rodinia 3.1 video volume (mutates seed[0] as Rodinia does)
    I_image = dhir_alloc_int(max_size);
    pf_video_sequence(I_image, IszX, IszY, Nfr, seed);

    // Copy mutated seed state to reference so both MLIR and Ref start identically
    memcpy(seed_ref, seed, sizeof(int) * Nparticles);

    // 4. Allocate MLIR tracking arrays
    arrayX = dhir_alloc_double(Nparticles);
    arrayY = dhir_alloc_double(Nparticles);
    weights = dhir_alloc_double(Nparticles);
    likelihood = dhir_alloc_double(Nparticles);
    CDF = dhir_alloc_double(Nparticles);
    u = dhir_alloc_double(Nparticles);
    xj = dhir_alloc_double(Nparticles);
    yj = dhir_alloc_double(Nparticles);
    out_xe = dhir_alloc_double(Nfr);
    out_ye = dhir_alloc_double(Nfr);

    // 5. Allocate Reference tracking arrays
    arrayX_ref = dhir_alloc_double(Nparticles);
    arrayY_ref = dhir_alloc_double(Nparticles);
    weights_ref = dhir_alloc_double(Nparticles);
    likelihood_ref = dhir_alloc_double(Nparticles);
    CDF_ref = dhir_alloc_double(Nparticles);
    u_ref = dhir_alloc_double(Nparticles);
    xj_ref = dhir_alloc_double(Nparticles);
    yj_ref = dhir_alloc_double(Nparticles);
    out_xe_ref = dhir_alloc_double(Nfr);
    out_ye_ref = dhir_alloc_double(Nfr);

    // 6. Initialize initial particle states (at frame 0 center)
    double x0 = roundDouble(IszY / 2.0);
    double y0 = roundDouble(IszX / 2.0);
    for (int i = 0; i < Nparticles; i++) {
        arrayX[i] = arrayX_ref[i] = x0;
        arrayY[i] = arrayY_ref[i] = y0;
        weights[i] = weights_ref[i] = 1.0 / (double)Nparticles;
        likelihood[i] = likelihood_ref[i] = 0.0;
        CDF[i] = CDF_ref[i] = 0.0;
        u[i] = u_ref[i] = 0.0;
        xj[i] = xj_ref[i] = 0.0;
        yj[i] = yj_ref[i] = 0.0;
    }
    for (int k = 0; k < Nfr; k++) {
        out_xe[k] = out_xe_ref[k] = 0.0;
        out_ye[k] = out_ye_ref[k] = 0.0;
    }
}

static void bench_call(void) {
    particle_filter(
        Nparticles, countOnes, max_size,
        IszX, IszY, Nfr,
        MR1(I_image, max_size),
        MR2(objxy, countOnes, 2),
        MR1(seed, Nparticles),
        MR1(arrayX, Nparticles),
        MR1(arrayY, Nparticles),
        MR1(weights, Nparticles),
        MR1(likelihood, Nparticles),
        MR1(CDF, Nparticles),
        MR1(u, Nparticles),
        MR1(xj, Nparticles),
        MR1(yj, Nparticles),
        MR1(out_xe, Nfr),
        MR1(out_ye, Nfr)
    );
}

static void bench_reference(void) {
    ref_particle_filter(
        Nparticles, countOnes, max_size,
        IszX, IszY, Nfr,
        I_image, objxy, seed_ref,
        arrayX_ref, arrayY_ref, weights_ref, likelihood_ref,
        CDF_ref, u_ref, xj_ref, yj_ref, out_xe_ref, out_ye_ref
    );
}

static int bench_check(void) {
    int errors = 0;

    // 1. Verify tracked object centroid trajectory across all processed frames (k = 1 ... Nfr-1)
    for (int k = 1; k < Nfr; k++) {
        double d_xe = std::fabs(out_xe[k] - out_xe_ref[k]);
        double d_ye = std::fabs(out_ye[k] - out_ye_ref[k]);
        if (d_xe > 1e-5 || d_ye > 1e-5) {
            if (errors < 5) {
                printf("  mismatch frame %d: MLIR (%.6f, %.6f) vs REF (%.6f, %.6f)\n",
                       k, out_xe[k], out_ye[k], out_xe_ref[k], out_ye_ref[k]);
            }
            errors++;
        }
    }

    // 2. Verify particle positions and weights
    for (int i = 0; i < Nparticles; i++) {
        double dx = std::fabs(arrayX[i] - arrayX_ref[i]);
        double dy = std::fabs(arrayY[i] - arrayY_ref[i]);
        double dw = std::fabs(weights[i] - weights_ref[i]);
        if (dx > 1e-5 || dy > 1e-5 || dw > 1e-5) {
            if (errors < 5) {
                printf("  mismatch particle %d: MLIR (%.4f, %.4f, %.6e) vs REF (%.4f, %.4f, %.6e)\n",
                       i, arrayX[i], arrayY[i], weights[i], arrayX_ref[i], arrayY_ref[i], weights_ref[i]);
            }
            errors++;
        }
    }

    return errors;
}

static void bench_free(void) {
    if (I_image) free(I_image);
    if (objxy) free(objxy);

    if (seed) free(seed);
    if (arrayX) free(arrayX);
    if (arrayY) free(arrayY);
    if (weights) free(weights);
    if (likelihood) free(likelihood);
    if (CDF) free(CDF);
    if (u) free(u);
    if (xj) free(xj);
    if (yj) free(yj);
    if (out_xe) free(out_xe);
    if (out_ye) free(out_ye);

    if (seed_ref) free(seed_ref);
    if (arrayX_ref) free(arrayX_ref);
    if (arrayY_ref) free(arrayY_ref);
    if (weights_ref) free(weights_ref);
    if (likelihood_ref) free(likelihood_ref);
    if (CDF_ref) free(CDF_ref);
    if (u_ref) free(u_ref);
    if (xj_ref) free(xj_ref);
    if (yj_ref) free(yj_ref);
    if (out_xe_ref) free(out_xe_ref);
    if (out_ye_ref) free(out_ye_ref);
}
