#define DHIR_BENCH_NAME "dwt2d"
#include "dhir_bench.h"

extern "C" {
void dwt2d(int32_t width, int32_t height, int32_t levels, int32_t components,
           int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
           int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
           int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t);

void ref_dwt2d(int width, int height, int levels, int components,
               const int *in, int *vert, int *out);
}

static int width = 0;
static int height = 0;
static int levels = 3; // Default 3 levels matching Rodinia run.sh -l 3
static int components = 3;
static long num_pixels = 0;
static long total_elements = 0;

static int *in_data = nullptr;
static int *vert = nullptr;
static int *out_data = nullptr;
static int *vert_ref = nullptr;
static int *out_ref = nullptr;

static void bench_alloc(void) {
    const char *candidates[] = {
        "vendor/rodinia/rodinia_3.1/data/dwt2d/rgb.bmp",
    };

    const char *filepath = getenv("DWT2D_INPUT");
    FILE *fp = nullptr;

    if (filepath) {
        fp = fopen(filepath, "rb");
    }
    if (!fp) {
        for (const char *cand : candidates) {
            fp = fopen(cand, "rb");
            if (fp) {
                filepath = cand;
                break;
            }
        }
    }

    if (!fp) {
        fprintf(stderr, "dwt2d: failed to open BMP dataset\n");
        exit(1);
    }

    // Read 54-byte BMP header to dynamically extract dimensions
    uint8_t header[54];
    if (fread(header, 1, 54, fp) != 54) {
        fprintf(stderr, "dwt2d: failed to read BMP header\n");
        fclose(fp);
        exit(1);
    }

    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "dwt2d: not a valid BMP file\n");
        fclose(fp);
        exit(1);
    }

    int32_t w = *(int32_t *)&header[18];
    int32_t h = *(int32_t *)&header[22];
    if (h < 0) h = -h;
    uint16_t bpp = *(uint16_t *)&header[28];
    components = (bpp > 0) ? (bpp / 8) : 3;

    width = w;
    height = h;
    num_pixels = (long)width * height;
    total_elements = (long)components * num_pixels;

    const char *env_levels = getenv("DWT2D_LEVELS");
    if (env_levels) {
        levels = atoi(env_levels);
    } else {
        levels = 3;
    }

    // Rewind to byte 0 matching Rodinia's raw byte stream loader
    fseek(fp, 0, SEEK_SET);
    size_t raw_size = (size_t)num_pixels * components;
    unsigned char *raw = (unsigned char *)malloc(raw_size);
    if (!raw) {
        fprintf(stderr, "dwt2d: memory allocation for raw BMP failed\n");
        fclose(fp);
        exit(1);
    }

    if (fread(raw, 1, raw_size, fp) != raw_size) {
        fprintf(stderr, "dwt2d: error reading BMP raw stream\n");
        free(raw);
        fclose(fp);
        exit(1);
    }
    fclose(fp);

    in_data = dhir_alloc_int(total_elements);
    vert = dhir_alloc_int(num_pixels);
    out_data = dhir_alloc_int(total_elements);
    vert_ref = dhir_alloc_int(num_pixels);
    out_ref = dhir_alloc_int(total_elements);

    // Extract all color components (R, G, B) centered by -128, matching Rodinia CUDA components.cu
    for (int c = 0; c < components; c++) {
        for (long i = 0; i < num_pixels; i++) {
            in_data[c * num_pixels + i] = (int)raw[i * components + c] - 128;
        }
    }
    free(raw);

    dhir_zero_int(vert, num_pixels);
    dhir_zero_int(out_data, total_elements);
    dhir_zero_int(vert_ref, num_pixels);
    dhir_zero_int(out_ref, total_elements);
}

static void bench_call(void) {
    dwt2d(width, height, levels, components,
          MR3(in_data, components, height, width),
          MR2(vert, height, width),
          MR3(out_data, components, height, width));
}

static void bench_reference(void) {
    ref_dwt2d(width, height, levels, components, in_data, vert_ref, out_ref);
}

static int bench_check(void) {
    return dhir_compare_int("out", out_data, out_ref, total_elements);
}

static void bench_free(void) {
    if (in_data) free(in_data);
    if (vert) free(vert);
    if (out_data) free(out_data);
    if (vert_ref) free(vert_ref);
    if (out_ref) free(out_ref);
}
