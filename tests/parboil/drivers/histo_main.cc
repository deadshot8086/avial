// MPI driver for the extracted Parboil histo kernel.

#define DHIR_BENCH_NAME "histo"
#include "dhir_bench.h"

extern "C" {
void histo(int32_t, int32_t, int32_t, int32_t, int32_t,
           uint32_t *, uint32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
           uint8_t *, uint8_t *, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_histo(int img_width, int img_height, int histo_width, int histo_height,
               int iters,
               const unsigned int *img, unsigned char *histo);
}

#include <string>

static int img_width = 0;
static int img_height = 0;
static int histo_width = 0;
static int histo_height = 0;
static int iters = 0;

static uint32_t *img = nullptr;
static uint8_t *bins = nullptr;      // written by the MLIR kernel
static uint8_t *bins_ref = nullptr;  // written by ref_histo

static long npixels(void) { return (long)img_width * (long)img_height; }
static long nbins(void) { return (long)histo_width * (long)histo_height; }

static void *xalloc(size_t bytes) {
    void *p = aligned_alloc(64, (bytes + 63) / 64 * 64);
    if (!p) {
        fprintf(stderr, "histo: allocation of %zu bytes failed\n", bytes);
        exit(2);
    }
    return p;
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
        snprintf(desc_path, sizeof(desc_path), "vendor/parboil/datasets/histo/default/input/DESCRIPTION");
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

static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "rb");
    if (fp) return fp;

    std::string p1 = std::string("vendor/parboil/datasets/histo/default/input/") + path;
    fp = fopen(p1.c_str(), "rb");
    return fp;
}

static bool read_dataset(const char *path) {
    FILE *f = open_resolved(path);
    if (!f) return false;

    unsigned int hdr[4];
    if (fread(hdr, sizeof(unsigned int), 4, f) != 4) {
        fclose(f);
        return false;
    }
    if (hdr[0] == 0 || hdr[1] == 0 || hdr[2] == 0 || hdr[3] == 0) {
        fclose(f);
        return false;
    }

    img_width = (int)hdr[0];
    img_height = (int)hdr[1];
    histo_width = (int)hdr[2];
    histo_height = (int)hdr[3];

    long n = npixels();
    img = (uint32_t *)xalloc((size_t)n * sizeof(uint32_t));
    if (fread(img, sizeof(unsigned int), (size_t)n, f) != (size_t)n) {
        fprintf(stderr, "[histo] short read of %ld pixels from %s\n", n, path);
        fclose(f);
        free(img);
        img = nullptr;
        return false;
    }
    fclose(f);

    fprintf(stderr, "[histo] dataset %s: img=%dx%d histo=%dx%d iters=%d\n",
            path, img_width, img_height, histo_width, histo_height, iters);
    return true;
}

static void synth_fallback(void) {
    const char *ew = getenv("HISTO_IMG_WIDTH");
    const char *eh = getenv("HISTO_IMG_HEIGHT");
    const char *bw = getenv("HISTO_WIDTH");
    const char *bh = getenv("HISTO_HEIGHT");
    img_width = ew ? atoi(ew) : 256;
    img_height = eh ? atoi(eh) : 256;
    histo_width = bw ? atoi(bw) : 64;
    histo_height = bh ? atoi(bh) : 4;
    if (img_width <= 0) img_width = 256;
    if (img_height <= 0) img_height = 256;
    if (histo_width <= 0) histo_width = 64;
    if (histo_height <= 0) histo_height = 4;

    long n = npixels();
    img = (uint32_t *)xalloc((size_t)n * sizeof(uint32_t));

    srand(7);
    long nb = nbins();
    for (long i = 0; i < n; ++i)
        img[i] = (uint32_t)(rand() % (int)nb);

    fprintf(stderr, "[histo] dataset not found, using synthetic data "
                    "(img=%dx%d histo=%dx%d iters=%d, srand(7))\n",
            img_width, img_height, histo_width, histo_height, iters);
}

static void bench_alloc(void) {
    const char *in = getenv("HISTO_INPUT");
    if (!in) in = "img.bin";

    char params[256] = {0};
    if (read_description_params(in, params, sizeof(params))) {
        sscanf(params, "%d", &iters);
    }
    if (iters <= 0) iters = 20;

    const char *ei = getenv("HISTO_ITERS");
    if (ei) {
        int v = atoi(ei);
        if (v > 0) iters = v;
    }

    if (!read_dataset(in))
        synth_fallback();

    long nb = nbins();
    bins = (uint8_t *)xalloc((size_t)nb);
    bins_ref = (uint8_t *)xalloc((size_t)nb);
    memset(bins, 0, (size_t)nb);
    memset(bins_ref, 0, (size_t)nb);
}

static void bench_call(void) {
    histo(img_width, img_height, histo_width, histo_height, iters,
          MR2(img, img_height, img_width),
          MR2(bins, histo_height, histo_width));
}

static void bench_reference(void) {
    ref_histo(img_width, img_height, histo_width, histo_height, iters,
              img, bins_ref);
}

static int compare_bins(const char *what, const uint8_t *got,
                        const uint8_t *want, long n) {
    int errors = 0;
    for (long i = 0; i < n; ++i) {
        if (got[i] != want[i]) {
            if (errors < 5)
                printf("  mismatch %s[%ld]: got %u want %u\n", what, i,
                       (unsigned)got[i], (unsigned)want[i]);
            ++errors;
        }
    }
    if (errors > 5) printf("  ... and %d more in %s\n", errors - 5, what);
    return errors;
}

static int bench_check(void) {
    return compare_bins("histo", bins, bins_ref, nbins());
}

static void bench_free(void) {
    if (img) free(img);
    if (bins) free(bins);
    if (bins_ref) free(bins_ref);
    img = nullptr;
    bins = bins_ref = nullptr;
}
