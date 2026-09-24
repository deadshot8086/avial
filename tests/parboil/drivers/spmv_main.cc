// MPI driver for the extracted Parboil spmv kernel.

#define DHIR_BENCH_NAME "spmv"
#include "dhir_bench.h"

extern "C" {
void spmv(int32_t, int32_t,
          int32_t *, int32_t *, int64_t, int64_t, int64_t,   // h_nzcnt
          int32_t *, int32_t *, int64_t, int64_t, int64_t,   // h_ptr
          int32_t *, int32_t *, int64_t, int64_t, int64_t,   // h_indices
          float *, float *, int64_t, int64_t, int64_t,       // h_data
          float *, float *, int64_t, int64_t, int64_t,       // h_x_vector
          int32_t *, int32_t *, int64_t, int64_t, int64_t,   // h_perm
          float *, float *, int64_t, int64_t, int64_t);      // h_Ax_vector

void ref_spmv(int dim, int iters, const int *h_nzcnt, const int *h_ptr,
              const int *h_indices, const float *h_data,
              const float *h_x_vector, const int *h_perm, float *h_Ax_vector);
}

#include <algorithm>
#include <string>
#include <vector>


static int dim = 0;         // number of rows/cols of the original matrix
static int len = 0;         // entries in h_data / h_indices
static int depth = 0;       // used entries in h_ptr
static int nzcnt_len = 0;   // entries in h_nzcnt / h_perm (JDS row count)
static int iters = 50;

static int *h_nzcnt = nullptr;
static int *h_ptr = nullptr;
static int *h_indices = nullptr;
static float *h_data = nullptr;
static float *h_x_vector = nullptr;
static int *h_perm = nullptr;
static float *h_Ax_vector = nullptr;
static float *h_Ax_ref = nullptr;

struct mat_entry {
    int row, col;
    float val;
};

struct row_stats {
    int index;
    int size;
    int start;
    int padding;
};

static int sort_rows(const void *a, const void *b) {
    return ((const mat_entry *)a)->row - ((const mat_entry *)b)->row;
}

// largest row first
static int sort_stats(const void *a, const void *b) {
    return ((const row_stats *)b)->size - ((const row_stats *)a)->size;
}


static FILE *open_resolved(const char *path, const char *mode) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, mode);
    if (fp) return fp;

    std::string p1 = std::string("vendor/parboil/datasets/spmv/medium/input/") + path;
    fp = fopen(p1.c_str(), mode);
    if (fp) return fp;

    std::string p2 = std::string("vendor/parboil/") + path;
    fp = fopen(p2.c_str(), mode);
    if (fp) return fp;

    return nullptr;
}


static bool read_coo(const char *path, std::vector<mat_entry> &entries, int *rows_out) {
    FILE *f = open_resolved(path, "r");
    if (!f) return false;

    char line[4096];
    if (!fgets(line, sizeof(line), f)) { fclose(f); return false; }
    bool binary = (strstr(line, "pattern") != nullptr);
    bool mirrored = (strstr(line, "symmetric") != nullptr);

    // skip comment lines
    do {
        if (!fgets(line, sizeof(line), f)) { fclose(f); return false; }
    } while (line[0] == '%');

    int rows = 0, cols = 0, nz = 0;
    if (sscanf(line, "%d %d %d", &rows, &cols, &nz) != 3 || rows <= 0 || nz <= 0) {
        fclose(f);
        return false;
    }

    entries.clear();
    entries.reserve(mirrored ? (size_t)2 * nz : (size_t)nz);
    for (int i = 0; i < nz; i++) {
        mat_entry e;
        if (binary) {
            if (fscanf(f, "%d %d", &e.row, &e.col) != 2) break;
            e.val = 1.0f;
        } else {
            if (fscanf(f, "%d %d %f", &e.row, &e.col, &e.val) != 3) break;
        }
        e.row--;
        e.col--;
        entries.push_back(e);
        if (mirrored && e.row != e.col) {
            mat_entry m;
            m.val = e.val;
            m.row = e.col;
            m.col = e.row;
            entries.push_back(m);
        }
    }
    fclose(f);

    if (entries.empty()) return false;
    *rows_out = rows;
    return true;
}

static void coo_to_jds(std::vector<mat_entry> &entries, int matrix_rows) {
    const int warp_size = 1, pack_size = 1;
    int pad_rows = 1 * pack_size;

    int nz = (int)entries.size();
    dim = matrix_rows;

    qsort(entries.data(), nz, sizeof(mat_entry), sort_rows);

    int rows = entries[nz - 1].row + 1;   // last item has greatest row
    if (rows % warp_size) rows += warp_size - rows % warp_size;

    std::vector<row_stats> stats(rows);
    for (int i = 0; i < rows; i++) stats[i] = row_stats{0, 0, 0, 0};

    int irow = entries[0].row, icol = 0, istart = 0;
    for (int i = 0; i < nz; i++) {
        if (entries[i].row != irow || i == nz - 1) {
            if (i == nz - 1) icol++;
            stats[irow].size = icol;
            stats[irow].index = entries[i - 1].row;
            stats[irow].start = istart;
            icol = 0;
            irow = entries[i].row;
            istart = i;
        }
        icol++;
    }

    nzcnt_len = rows / warp_size + rows % warp_size;

    int meta_len = nzcnt_len > dim ? nzcnt_len : dim;

    h_nzcnt = dhir_alloc_int(meta_len);
    h_perm = dhir_alloc_int(meta_len);
    dhir_zero_int(h_nzcnt, meta_len);
    dhir_zero_int(h_perm, meta_len);

    qsort(stats.data(), rows, sizeof(row_stats), sort_stats);

    int total_size = 0, pad_to = 0, pack_to;
    for (int i = 0; i < rows; i++) {
        h_perm[i] = stats[i].index;
        if (i % warp_size == 0) {
            stats[i].padding = (stats[i].size % pad_rows)
                                   ? pad_rows - (stats[i].size % pad_rows)
                                   : 0;
            pack_to = (stats[i].size % pack_size)
                          ? (stats[i].size + pack_size - 1) / pack_size
                          : stats[i].size / pack_size;
            pad_to = stats[i].size + stats[i].padding;
            h_nzcnt[i / warp_size] = pack_to;
            total_size += pad_to * warp_size;
        } else {
            stats[i].padding = pad_to - stats[i].size;
        }
    }

    len = total_size;
    h_data = dhir_alloc(len);
    h_indices = dhir_alloc_int(len);
    dhir_zero(h_data, len);
    dhir_zero_int(h_indices, len);

    h_ptr = dhir_alloc_int(rows + 1);
    dhir_zero_int(h_ptr, rows + 1);

    // Fubini / transpose write-out
    int jrow = 0, idata = 0;
    while (1) {
        h_ptr[jrow] = idata;
        if (stats[0].size + stats[0].padding <= jrow * pack_size) break;
        for (int i = 0; i < rows; i++) {
            for (int ipack = 0; ipack < pack_size; ipack++) {
                if (stats[i].size > jrow * pack_size + ipack) {
                    int entry_index = stats[i].start + jrow * pack_size + ipack;
                    h_data[idata] = entries[entry_index].val;
                    h_indices[idata] = entries[entry_index].col;
                } else if (stats[i].size + stats[i].padding >
                           jrow * pack_size + ipack) {
                    h_indices[idata] = 0;   // padding slot; value stays 0.0f
                } else {
                    goto endwrite;
                }
                idata++;
            }
        }
    endwrite:
        jrow++;
    }

    depth = jrow + 1;
}

static void synth_coo(std::vector<mat_entry> &entries, int n, int band) {
    srand(7);
    entries.clear();
    for (int r = 0; r < n; r++) {
        for (int b = -band; b <= band; b++) {
            int c = r + b;
            if (c < 0 || c >= n) continue;
            mat_entry e;
            e.row = r;
            e.col = c;
            e.val = (float)((rand() % 2000) - 1000) / 100.0f;
            if (b == 0) e.val += (float)(2 * band + 1);
            entries.push_back(e);
        }
    }
}

static void read_description_params(const char *mfile) {
    std::string path(mfile);
    size_t slash = path.find_last_of('/');
    std::string dir = (slash != std::string::npos) ? path.substr(0, slash) : ".";
    std::string desc = dir + "/DESCRIPTION";
    FILE *f = fopen(desc.c_str(), "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "Parameters:", 11) == 0) {
            char *p = strstr(line, "--");
            if (p) {
                int it = 0;
                if (sscanf(p + 2, "%d", &it) == 1 && it > 0) {
                    iters = it;
                }
            }
        }
    }
    fclose(f);
}

static void bench_alloc(void) {
    const char *mfile = getenv("SPMV_INPUT");
    if (!mfile) mfile = "vendor/parboil/datasets/spmv/medium/input/bcsstk18.mtx";
    const char *vfile = getenv("SPMV_VECTOR");
    if (!vfile) vfile = "vendor/parboil/datasets/spmv/medium/input/vector.bin";

    read_description_params(mfile);

    if (const char *it = getenv("SPMV_ITERS")) {
        int v = atoi(it);
        if (v > 0) iters = v;
    }

    std::vector<mat_entry> entries;
    int matrix_rows = 0;
    bool real_data = read_coo(mfile, entries, &matrix_rows);

    if (!real_data) {
        int n = 1138;
        if (const char *d = getenv("SPMV_DIM")) {
            int v = atoi(d);
            if (v > 2) n = v;
        }
        fprintf(stderr, "[spmv] dataset %s unavailable, using synthetic %dx%d\n",
                mfile, n, n);
        synth_coo(entries, n, 3);
        matrix_rows = n;
    }

    coo_to_jds(entries, matrix_rows);

    h_x_vector = dhir_alloc(dim);
    h_Ax_vector = dhir_alloc(dim);
    h_Ax_ref = dhir_alloc(dim);

    bool got_vec = false;
    if (real_data) {
        FILE *fv = open_resolved(vfile, "rb");
        if (fv) {
            got_vec = (fread(h_x_vector, sizeof(float), dim, fv) == (size_t)dim);
            fclose(fv);
        }
    }
    if (!got_vec) {
        srand(7);
        for (int i = 0; i < dim; i++)
            h_x_vector[i] = (float)((rand() % 1000) - 500) / 500.0f;
    }

    dhir_zero(h_Ax_vector, dim);
    dhir_zero(h_Ax_ref, dim);

    printf("[spmv] %s dim=%d nz_padded=%d depth=%d nzcnt_len=%d iters=%d\n",
           real_data ? "dataset" : "synthetic", dim, len, depth, nzcnt_len,
           iters);
}

static void bench_call(void) {
    spmv(dim, iters,
         MR1(h_nzcnt, dim),
         MR1(h_ptr, depth),
         MR1(h_indices, len),
         MR1(h_data, len),
         MR1(h_x_vector, dim),
         MR1(h_perm, dim),
         MR1(h_Ax_vector, dim));
}

static void bench_reference(void) {
    ref_spmv(dim, iters, h_nzcnt, h_ptr, h_indices, h_data, h_x_vector, h_perm,
             h_Ax_ref);
}

static int bench_check(void) {
    return dhir_compare("Ax", h_Ax_vector, h_Ax_ref, dim, DHIR_EPS);
}

static void bench_free(void) {
    free(h_nzcnt);
    free(h_ptr);
    free(h_indices);
    free(h_data);
    free(h_x_vector);
    free(h_perm);
    free(h_Ax_vector);
    free(h_Ax_ref);
    h_nzcnt = h_ptr = h_indices = h_perm = nullptr;
    h_data = h_x_vector = h_Ax_vector = h_Ax_ref = nullptr;
}
