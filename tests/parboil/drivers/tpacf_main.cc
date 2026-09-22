// MPI driver for the extracted Parboil tpacf kernel.

#define DHIR_BENCH_NAME "tpacf"
#include "dhir_bench.h"

extern "C" {
void tpacf(int32_t, int32_t, int32_t,                                     // npoints, random_count, nbins
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, // data (MR2)
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, // randoms (MR3)
           float *, float *, int64_t, int64_t, int64_t,                   // binb (MR1)
           int64_t *, int64_t *, int64_t, int64_t, int64_t,               // DD (MR1)
           int64_t *, int64_t *, int64_t, int64_t, int64_t,               // RRS (MR1)
           int64_t *, int64_t *, int64_t, int64_t, int64_t);              // DRS (MR1)

void ref_tpacf(int npoints, int random_count, int nbins,
               const float *data, const float *randoms, const float *binb,
               int64_t *DD, int64_t *RRS, int64_t *DRS);
}

#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TPACF_BINS_PER_DEC 5
#define TPACF_MIN_ARCMIN 1.0
#define TPACF_MAX_ARCMIN 10000.0

static int npoints = 0;
static int random_count = 0;
static int nbins = 0;

static float *binb = nullptr;    /* [nbins+1] */
static float *data = nullptr;    /* [npoints][3]                 -- the D set */
static float *randoms = nullptr; /* [random_count][npoints][3]   -- the R sets */

static int64_t *DD = nullptr, *RRS = nullptr, *DRS = nullptr;
static int64_t *DD_ref = nullptr, *RRS_ref = nullptr, *DRS_ref = nullptr;

static bool real_data = false;

static std::string input_dir;

static void resolve_input_dir(void) {
    const char *env = getenv("TPACF_INPUT");
    std::string candidates[5];
    int n = 0;
    if (env && *env) {
        candidates[n++] = env;
        std::string s(env);
        size_t slash = s.find_last_of('/');
        if (slash != std::string::npos) candidates[n++] = s.substr(0, slash);
    }
    candidates[n++] = "vendor/parboil/datasets/tpacf/small/input";

    for (int i = 0; i < n; ++i) {
        std::string probe = candidates[i] + "/Datapnts.1";
        FILE *f = fopen(probe.c_str(), "r");
        if (f) {
            fclose(f);
            input_dir = candidates[i];
            return;
        }
    }
    input_dir.clear();
}

static int readdatafile(const char *fname, float *out, int want) {
    FILE *infile = fopen(fname, "r");
    int lcount = 0;
    float ra, dec;

    if (!infile) return 0;

    for (lcount = 0; lcount < want; lcount++) {
        if (fscanf(infile, "%f %f", &ra, &dec) != 2) break;
        {
            float rarad = (float)(M_PI / 180.0 * (double)ra);
            float decrad = (float)(M_PI / 180.0 * (double)dec);
            float cd = (float)cos((double)decrad);

            out[lcount * 3 + 0] = (float)(cos((double)rarad) * (double)cd);
            out[lcount * 3 + 1] = (float)(sin((double)rarad) * (double)cd);
            out[lcount * 3 + 2] = (float)sin((double)decrad);
        }
    }

    fclose(infile);
    return lcount;
}

static void synth_pair(float *out, int i, double ra_lo, double ra_hi,
                       double dec_lo, double dec_hi) {
    double u = (double)(rand() % 1000000) / 1000000.0;
    double v = (double)(rand() % 1000000) / 1000000.0;
    float ra = (float)(ra_lo + u * (ra_hi - ra_lo));
    float dec = (float)(dec_lo + v * (dec_hi - dec_lo));

    float rarad = (float)(M_PI / 180.0 * (double)ra);
    float decrad = (float)(M_PI / 180.0 * (double)dec);
    float cd = (float)cos((double)decrad);

    out[i * 3 + 0] = (float)(cos((double)rarad) * (double)cd);
    out[i * 3 + 1] = (float)(sin((double)rarad) * (double)cd);
    out[i * 3 + 2] = (float)sin((double)decrad);
}

static void synth_fill(void) {
    srand(7);
    for (int i = 0; i < npoints; ++i)
        synth_pair(data, i, 0.0, 2.2, -11.3, 1.3);
    for (int rf = 0; rf < random_count; ++rf) {
        float *r = randoms + (size_t)rf * npoints * 3;
        for (int i = 0; i < npoints; ++i)
            synth_pair(r, i, 0.0, 360.0, -11.0, 68.0);
    }
}

static int count_file_lines(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return 0;
    int count = 0;
    float ra, dec;
    while (fscanf(f, "%f %f", &ra, &dec) == 2) {
        count++;
    }
    fclose(f);
    return count;
}

static void read_description_params(const char *dir, int *p_rand_count, int *p_npts) {
    std::string desc_path = std::string(dir) + "/DESCRIPTION";
    FILE *f = fopen(desc_path.c_str(), "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "Parameters:", 11) == 0) {
            char *p = strstr(line, "--");
            if (p) {
                char *pn = strstr(p, "-n");
                if (pn && p_rand_count) {
                    int val = 0;
                    if (sscanf(pn + 2, "%d", &val) == 1 && val > 0) *p_rand_count = val;
                }
                char *pp = strstr(p, "-p");
                if (pp && p_npts) {
                    int val = 0;
                    if (sscanf(pp + 2, "%d", &val) == 1 && val > 0) *p_npts = val;
                }
            }
        }
    }
    fclose(f);
}


static void bench_alloc(void) {
    resolve_input_dir();
    real_data = !input_dir.empty();

    if (real_data) {
        read_description_params(input_dir.c_str(), &random_count, &npoints);
        if (npoints <= 0) {
            std::string dp = input_dir + "/Datapnts.1";
            npoints = count_file_lines(dp.c_str());
        }
        if (random_count <= 0) {
            for (int r = 1; ; ++r) {
                char name[64];
                snprintf(name, sizeof(name), "/Randompnts.%d", r);
                std::string rp = input_dir + name;
                FILE *f = fopen(rp.c_str(), "r");
                if (!f) break;
                fclose(f);
                random_count++;
            }
        }
    }

    if (npoints <= 0) npoints = 487;
    if (random_count <= 0) random_count = 100;

    if (const char *e = getenv("TPACF_NPOINTS")) {
        int v = atoi(e);
        if (v > 1) npoints = v;
    }
    if (const char *e = getenv("TPACF_RANDOM_COUNT")) {
        int v = atoi(e);
        if (v >= 0) random_count = v;
    }

    nbins = (int)floor(TPACF_BINS_PER_DEC *
                       (log10(TPACF_MAX_ARCMIN) - log10(TPACF_MIN_ARCMIN)));

    binb = dhir_alloc(nbins + 1);
    for (int k = 0; k < nbins + 1; k++) {
        binb[k] = (float)cos(pow(10.0, log10(TPACF_MIN_ARCMIN) +
                                           k * 1.0 / TPACF_BINS_PER_DEC) /
                             60.0 * M_PI / 180.0);
    }

    data = dhir_alloc((long)npoints * 3);
    randoms = dhir_alloc((long)random_count * npoints * 3 + 3);

    if (real_data) {
        std::string dp = input_dir + "/Datapnts.1";
        if (readdatafile(dp.c_str(), data, npoints) != npoints) {
            fprintf(stderr, "[tpacf] short read of %s, using synthetic data\n",
                    dp.c_str());
            real_data = false;
        }
    }
    for (int rf = 0; rf < random_count && real_data; ++rf) {
        char name[64];
        snprintf(name, sizeof(name), "/Randompnts.%d", rf + 1);
        std::string rp = input_dir + name;
        if (readdatafile(rp.c_str(), randoms + (size_t)rf * npoints * 3,
                         npoints) != npoints) {
            fprintf(stderr, "[tpacf] short read of %s, using synthetic data\n",
                    rp.c_str());
            real_data = false;
        }
    }

    if (!real_data) {
        fprintf(stderr,
                "[tpacf] dataset unavailable, using synthetic data "
                "(npoints=%d random_count=%d, srand(7))\n",
                npoints, random_count);
        synth_fill();
    }

    long nb = nbins + 2;
    DD = dhir_alloc_int64(nb);
    RRS = dhir_alloc_int64(nb);
    DRS = dhir_alloc_int64(nb);
    DD_ref = dhir_alloc_int64(nb);
    RRS_ref = dhir_alloc_int64(nb);
    DRS_ref = dhir_alloc_int64(nb);

    printf("[tpacf] %s npoints=%d random_count=%d nbins=%d (%lld pairs)\n",
           real_data ? "dataset" : "synthetic", npoints, random_count, nbins,
           (long long)npoints * (npoints - 1) / 2 +
               (long long)random_count *
                   ((long long)npoints * (npoints - 1) / 2 +
                    (long long)npoints * npoints));
}

static void bench_call(void) {
    const long nb = nbins + 2;
    tpacf(npoints, random_count, nbins,
          MR2(data, npoints, 3),
          MR3(randoms, random_count, npoints, 3),
          MR1(binb, nbins + 1),
          MR1(DD, nb),
          MR1(RRS, nb),
          MR1(DRS, nb));
}

static void bench_reference(void) {
    ref_tpacf(npoints, random_count, nbins,
              data, randoms, binb,
              DD_ref, RRS_ref, DRS_ref);
}

static int bench_check(void) {
    const long nb = nbins + 2;
    int errors = dhir_compare_int64("DD", DD, DD_ref, nb);
    errors += dhir_compare_int64("DRS", DRS, DRS_ref, nb);
    errors += dhir_compare_int64("RRS", RRS, RRS_ref, nb);
    return errors;
}

static void bench_free(void) {
    free(binb);
    free(data);
    free(randoms);
    free(DD);
    free(RRS);
    free(DRS);
    free(DD_ref);
    free(RRS_ref);
    free(DRS_ref);
    binb = data = randoms = nullptr;
    DD = RRS = DRS = DD_ref = RRS_ref = DRS_ref = nullptr;
}
