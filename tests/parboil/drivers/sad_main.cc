// MPI driver for the extracted Parboil sad kernel.

#define DHIR_BENCH_NAME "sad"
#include "dhir_bench.h"

extern "C" {
void sad(int32_t, int32_t, int32_t, int32_t,
         uint16_t *, uint16_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
         uint16_t *, uint16_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
         uint16_t *, uint16_t *, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_sad(int mb_width, int mb_height, int height, int width,
             const unsigned short *cur, const unsigned short *ref,
             unsigned short *sads);
}

#include <string>

#define SAD_MAX_POS_PADDED 1092
#define SAD_MAX_POS 1089
// Per-macroblock slab width, matching sad.c's SAD_SLAB.
#define SAD_SLAB (41 * SAD_MAX_POS_PADDED)
// Local (within-slab) vector-base offsets per block-type region, mb-major.
#define SAD_L7 0
#define SAD_L6 (16 * SAD_MAX_POS_PADDED)
#define SAD_L5 (24 * SAD_MAX_POS_PADDED)
#define SAD_L4 (32 * SAD_MAX_POS_PADDED)
#define SAD_L3 (36 * SAD_MAX_POS_PADDED)
#define SAD_L2 (38 * SAD_MAX_POS_PADDED)
#define SAD_L1 (40 * SAD_MAX_POS_PADDED)

static int width = 0;
static int height = 0;
static int mb_width = 0;
static int mb_height = 0;

static uint16_t *cur_img = nullptr;   // current frame  (frame.bin)
static uint16_t *ref_img = nullptr;   // reference frame (reference.bin)
static uint16_t *sads = nullptr;      // written by the MLIR kernel
static uint16_t *sads_ref = nullptr;  // written by ref_sad

static long npixels(void) { return (long)width * (long)height; }

static long nsads(void) {
    return 41L * SAD_MAX_POS_PADDED * (long)mb_width * (long)mb_height;
}

static void *xalloc(size_t bytes) {
    void *p = aligned_alloc(64, (bytes + 63) / 64 * 64);
    if (!p) {
        fprintf(stderr, "sad: allocation of %zu bytes failed\n", bytes);
        exit(2);
    }
    return p;
}

// path resolution, mirroring histo_main.cc / spmv_main.cc
static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "rb");
    if (fp) return fp;

    std::string p1 =
        std::string("vendor/parboil/datasets/sad/default/input/") + path;
    fp = fopen(p1.c_str(), "rb");
    return fp;
}

// Port of image.c's load_image: two 16-bit dimensions then width*height pixels,
// all little-endian (file.c's read16u).
static uint16_t *load_image(const char *path, int *w_out, int *h_out) {
    FILE *f = open_resolved(path);
    if (!f) return nullptr;

    unsigned char hdr[4];
    if (fread(hdr, 1, 4, f) != 4) {
        fclose(f);
        return nullptr;
    }
    int w = (int)hdr[0] + ((int)hdr[1] << 8);
    int h = (int)hdr[2] + ((int)hdr[3] << 8);
    if (w <= 0 || h <= 0) {
        fclose(f);
        return nullptr;
    }

    size_t n = (size_t)w * (size_t)h;
    uint16_t *data = (uint16_t *)xalloc(n * sizeof(uint16_t));
    if (fread(data, sizeof(uint16_t), n, f) != n) {
        fprintf(stderr, "[sad] short read of %zu pixels from %s\n", n, path);
        fclose(f);
        free(data);
        return nullptr;
    }
    fclose(f);

    *w_out = w;
    *h_out = h;
    return data;
}


static void synth_fallback(void) {
    const char *ew = getenv("SAD_WIDTH");
    const char *eh = getenv("SAD_HEIGHT");
    width = ew ? atoi(ew) : 176;
    height = eh ? atoi(eh) : 144;
    if (width <= 0) width = 176;
    if (height <= 0) height = 144;
    width = (width / 16) * 16;
    height = (height / 16) * 16;
    if (width < 16) width = 16;
    if (height < 16) height = 16;

    long n = (long)width * (long)height;
    cur_img = (uint16_t *)xalloc((size_t)n * sizeof(uint16_t));
    ref_img = (uint16_t *)xalloc((size_t)n * sizeof(uint16_t));

    srand(7);
    for (long i = 0; i < n; ++i) {
        int v = rand() % 256;
        ref_img[i] = (uint16_t)v;
        // shifted-and-perturbed copy: same dynamic range as the real frames
        int d = (rand() % 17) - 8;
        int c = v + d;
        if (c < 0) c = 0;
        if (c > 255) c = 255;
        cur_img[i] = (uint16_t)c;
    }

    fprintf(stderr,
            "[sad] dataset not found, using synthetic frames "
            "(%dx%d, srand(7))\n",
            width, height);
}

static void bench_alloc(void) {
    const char *cf = getenv("SAD_INPUT");
    if (!cf) cf = "frame.bin";
    const char *rf = getenv("SAD_REF_INPUT");
    if (!rf) rf = "reference.bin";

    int cw = 0, ch = 0, rw = 0, rh = 0;
    cur_img = load_image(cf, &cw, &ch);
    ref_img = load_image(rf, &rw, &rh);

    bool real_data = (cur_img && ref_img && cw == rw && ch == rh &&
                      (cw % 16) == 0 && (ch % 16) == 0);
    if (real_data) {
        width = cw;
        height = ch;
        fprintf(stderr, "[sad] dataset %s + %s: %dx%d pixels\n", rf, cf, width,
                height);
    } else {
        if (cur_img) {
            free(cur_img);
            cur_img = nullptr;
        }
        if (ref_img) {
            free(ref_img);
            ref_img = nullptr;
        }
        synth_fallback();
    }

    mb_width = width / 16;
    mb_height = height / 16;

    long ns = nsads();
    sads = (uint16_t *)xalloc((size_t)ns * sizeof(uint16_t));
    sads_ref = (uint16_t *)xalloc((size_t)ns * sizeof(uint16_t));
    memset(sads, 0, (size_t)ns * sizeof(uint16_t));
    memset(sads_ref, 0, (size_t)ns * sizeof(uint16_t));

    printf("[sad] %s image=%dx%d macroblocks=%dx%d (%d) sads=%ld\n",
           real_data ? "dataset" : "synthetic", width, height, mb_width,
           mb_height, mb_width * mb_height, ns);
    (void)npixels;
}

static void bench_call(void) {
    sad(mb_width, mb_height, height, width,
        MR2(cur_img, height, width),
        MR2(ref_img, height, width),
        MR2(sads, (long)mb_width * mb_height, SAD_SLAB));
}

static void bench_reference(void) {
    ref_sad(mb_width, mb_height, height, width, cur_img, ref_img, sads_ref);
}


static int compare_sads(const char *what, const uint16_t *got,
                        const uint16_t *want, long n) {
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
    const long mbs = (long)mb_width * mb_height;
    const long MPP = SAD_MAX_POS_PADDED;
    struct { long ref_base, loc_base; int nvec; } region[7] = {
        {0L,                 SAD_L1, 1},   // type 1
        {mbs * MPP,          SAD_L2, 2},   // type 2
        {mbs * 3 * MPP,      SAD_L3, 2},   // type 3
        {mbs * 5 * MPP,      SAD_L4, 4},   // type 4
        {mbs * 9 * MPP,      SAD_L5, 8},   // type 5
        {mbs * 17 * MPP,     SAD_L6, 8},   // type 6
        {mbs * 25 * MPP,     SAD_L7, 16},  // type 7
    };
    int errors = 0;
    for (int r = 0; r < 7; ++r) {
        for (long mb = 0; mb < mbs; ++mb) {
            for (int v = 0; v < region[r].nvec; ++v) {
                for (int pos = 0; pos < SAD_MAX_POS; ++pos) {
                    long ref_idx = region[r].ref_base +
                                   mb * (region[r].nvec * MPP) + v * MPP + pos;
                    long ker_idx = mb * SAD_SLAB + region[r].loc_base +
                                   v * MPP + pos;
                    if (sads[ker_idx] != sads_ref[ref_idx]) {
                        if (errors < 5)
                            printf("  mismatch type%d mb=%ld vec=%d pos=%d: "
                                   "got %u want %u\n",
                                   r + 1, mb, v, pos, (unsigned)sads[ker_idx],
                                   (unsigned)sads_ref[ref_idx]);
                        ++errors;
                    }
                }
            }
        }
    }
    if (errors > 5) printf("  ... and %d more in sads\n", errors - 5);
    (void)compare_sads;
    return errors;
}

static void bench_free(void) {
    if (cur_img) free(cur_img);
    if (ref_img) free(ref_img);
    if (sads) free(sads);
    if (sads_ref) free(sads_ref);
    cur_img = ref_img = nullptr;
    sads = sads_ref = nullptr;
}
