// MPI driver for the extracted Parboil lbm kernel (one D3Q19 stream-collide
// sweep per call).

#define DHIR_BENCH_NAME "lbm"
#include "dhir_bench.h"


extern "C" {
void lbm(int32_t, int32_t, int32_t, int32_t,
         /* src: memref<?x?x?x20xf64> */
         double *, double *, int64_t,
         int64_t, int64_t, int64_t, int64_t,
         int64_t, int64_t, int64_t, int64_t,
         /* dst: memref<?x?x?x20xf64> */
         double *, double *, int64_t,
         int64_t, int64_t, int64_t, int64_t,
         int64_t, int64_t, int64_t, int64_t,
         /* flag: memref<?x?x?xi32> */
         int *, int *, int64_t,
         int64_t, int64_t, int64_t,
         int64_t, int64_t, int64_t);

void ref_lbm(int nzt, int ny, int nx, int steps,
             double *src, double *dst,
             const int *flag);
}


#define NCE 20 
#define MARGIN_PLANES 2 

#define DFL1 (1.0 / 3.0)
#define DFL2 (1.0 / 18.0)
#define DFL3 (1.0 / 36.0)

#define F_OBSTACLE (1 << 0)
#define F_ACCEL (1 << 1)


static int nx = 0;
static int ny = 0;
static int nz = 0;
static int nzt = 0;
static int steps = 0;

static long cells = 0;  // nzt * ny * nx
static long total = 0;  // cells * NCE

static double *grid_a = nullptr, *grid_b = nullptr;
static double *grid_a_ref = nullptr, *grid_b_ref = nullptr;
static int *flag = nullptr;  // shared: read-only for both kernel and reference
static double *out = nullptr, *out_ref = nullptr;

static int my_rank = 0;
static int used_dataset = 0;


static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "rb");
    if (fp) return fp;

    char buf[1024];
    snprintf(buf, sizeof(buf), "vendor/parboil/datasets/lbm/short/input/%s", path);
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
        snprintf(desc_path, sizeof(desc_path), "vendor/parboil/datasets/lbm/short/input/DESCRIPTION");
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


static void init_grid(double *g) {
    for (long c = 0; c < cells; ++c) {
        double *p = g + c * NCE;
        p[0] = DFL1;
        for (int e = 1; e <= 6; ++e) p[e] = DFL2;
        for (int e = 7; e <= 18; ++e) p[e] = DFL3;
        p[19] = 0.0;
    }
}

static int load_obstacle_file(const char *path) {
    FILE *fp = open_resolved(path);
    if (!fp) return 0;

    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return 0; }
    long fsz = ftell(fp);
    rewind(fp);

    long zstride = (long)ny * (nx + 1) + 1;
    if (zstride <= 0 || fsz <= 0 || fsz % zstride != 0 || fsz / zstride < nz) {
        if (my_rank == 0)
            fprintf(stderr,
                    "[lbm] %s: size %ld does not match a %dx%dx>=%d LDC geometry "
                    "(expected a multiple of %ld), using synthetic obstacles\n",
                    path, fsz, nx, ny, nz, zstride);
        fclose(fp);
        return 0;
    }

    for (int z = 0; z < nz; ++z) {
        for (int y = 0; y < ny; ++y) {
            for (int x = 0; x < nx; ++x) {
                int ch = fgetc(fp);
                if (ch == EOF) { fclose(fp); return 0; }
                if (ch != '.')
                    flag[(long)(z + MARGIN_PLANES) * ny * nx + (long)y * nx + x] |= F_OBSTACLE;
            }
            fgetc(fp); /* row separator */
        }
        fgetc(fp); /* plane separator */
    }
    fclose(fp);
    return 1;
}


static void synth_obstacles(void) {
    srand(7);
    for (int z = 0; z < nz; ++z)
        for (int y = 0; y < ny; ++y)
            for (int x = 0; x < nx; ++x)
                if (rand() % 1000 < 113)
                    flag[(long)(z + MARGIN_PLANES) * ny * nx + (long)y * nx + x] |= F_OBSTACLE;
}


static void init_special_cells_ldc(void) {
    for (int z = -MARGIN_PLANES; z < nz + MARGIN_PLANES; ++z) {
        for (int y = 0; y < ny; ++y) {
            for (int x = 0; x < nx; ++x) {
                long idx = (long)(z + MARGIN_PLANES) * ny * nx + (long)y * nx + x;
                if (x == 0 || x == nx - 1 || y == 0 || y == ny - 1 || z == 0 ||
                    z == nz - 1) {
                    flag[idx] |= F_OBSTACLE;
                } else if ((z == 1 || z == nz - 2) && x > 1 && x < nx - 2 &&
                           y > 1 && y < ny - 2) {
                    flag[idx] |= F_ACCEL;
                }
            }
        }
    }
}


static void bench_alloc(void) {
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    const char *input = getenv("LBM_INPUT");
    if (!input) input = "vendor/parboil/datasets/lbm/short/input/120_120_150_ldc.of";

    char params[256] = {0};
    if (read_description_params(input, params, sizeof(params))) {
        sscanf(params, "%d", &steps);
    }
    const char *base = strrchr(input, '/');
    base = base ? base + 1 : input;
    int fx = 0, fy = 0, fz = 0;
    if (sscanf(base, "%d_%d_%d", &fx, &fy, &fz) == 3) {
        nx = fx;
        ny = fy;
        nz = fz;
    }

    const char *e;
    if ((e = getenv("LBM_NX"))) nx = atoi(e);
    if ((e = getenv("LBM_NY"))) ny = atoi(e);
    if ((e = getenv("LBM_NZ"))) nz = atoi(e);
    if ((e = getenv("LBM_STEPS"))) steps = atoi(e);
    if (nx < 5) nx = 120;
    if (ny < 5) ny = 120;
    if (nz < 5) nz = 150;
    if (steps < 1) steps = 100;

    nzt = nz + 2 * MARGIN_PLANES;
    cells = (long)nzt * ny * nx;
    total = cells * NCE;

    grid_a = dhir_alloc_double(total);
    grid_b = dhir_alloc_double(total);
    flag = dhir_alloc_int(cells);
    if (my_rank == 0) {
        grid_a_ref = dhir_alloc_double(total);
        grid_b_ref = dhir_alloc_double(total);
    }


    init_grid(grid_a);
    init_grid(grid_b);
    dhir_zero_int(flag, cells);

    used_dataset = load_obstacle_file(input);
    if (!used_dataset) synth_obstacles();

    init_special_cells_ldc();

    if (my_rank == 0) {
        long nobs = 0, nacc = 0;
        for (int z = 0; z < nz; ++z)
            for (int y = 0; y < ny; ++y)
                for (int x = 0; x < nx; ++x) {
                    int f = flag[(long)(z + MARGIN_PLANES) * ny * nx + (long)y * nx + x];
                    if (f & F_OBSTACLE) ++nobs;
                    else if (f & F_ACCEL) ++nacc;
                }
        printf("lbm: %dx%dx%d (z extent %d incl. 2+2 ghost planes), %d timesteps, "
               "data=%s\n"
               "lbm: obstacle cells %ld, accel cells %ld, fluid cells %ld, "
               "%.1f MB/grid\n",
               nx, ny, nz, nzt, steps, used_dataset ? "dataset" : "synthetic",
               nobs, nacc, (long)nx * ny * nz - nobs - nacc,
               total * sizeof(double) / (1024.0 * 1024.0));
    }

    if (my_rank == 0) {
        memcpy(grid_a_ref, grid_a, (size_t)total * sizeof(double));
        memcpy(grid_b_ref, grid_b, (size_t)total * sizeof(double));
    }
}

static void bench_call(void) {
    lbm(nzt, ny, nx, steps,
        MR4(grid_a, nzt, ny, nx, NCE),
        MR4(grid_b, nzt, ny, nx, NCE),
        MR3(flag, nzt, ny, nx));
}

static void bench_reference(void) {
    ref_lbm(nzt, ny, nx, steps, grid_a_ref, grid_b_ref, flag);
}

static int bench_check(void) {
    out = (steps % 2 == 1) ? grid_b : grid_a;
    out_ref = (steps % 2 == 1) ? grid_b_ref : grid_a_ref;
    // Report the real margin, so a loosened tolerance could not hide anything.
    double maxrel = 0.0, maxabs = 0.0;
    for (long i = 0; i < total; ++i) {
        double w = out_ref[i], g = out[i];
        double a = std::fabs(g - w);
        double r = a / (std::fabs(w) > 1.0 ? std::fabs(w) : 1.0);
        if (a > maxabs) maxabs = a;
        if (r > maxrel) maxrel = r;
    }
    printf("lbm: max abs diff %.3e, max rel diff %.3e (tolerance %.3e)\n",
           maxabs, maxrel, (double)DHIR_EPS);
    return dhir_compare_double("grid", out, out_ref, total, DHIR_EPS);
}

static void bench_free(void) {
    if (grid_a) free(grid_a);
    if (grid_b) free(grid_b);
    if (grid_a_ref) free(grid_a_ref);
    if (grid_b_ref) free(grid_b_ref);
    if (flag) free(flag);
    grid_a = grid_b = grid_a_ref = grid_b_ref = nullptr;
    flag = nullptr;
}
