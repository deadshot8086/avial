// MPI driver for the extracted Parboil cutcp kernel.

#define DHIR_BENCH_NAME "cutcp"
#include "dhir_bench.h"

#include <vector>

extern "C" {
void cutcp(int32_t,
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
           int32_t, int32_t, int32_t,
           float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
           int64_t, int64_t,
           float, float, float,
           float, float,
           int32_t,
           float, float);

void ref_cutcp(int natoms, const float *atoms,
               int nx, int ny, int nz, float *lattice,
               float xlo, float ylo, float zlo,
               float gridspacing, float inv_gridspacing, int radius,
               float a2, float inv_a2);
}


static float gridspacing = 0.5f;
static float cutoff = 12.0f;
static float padding = 0.5f;

static int natoms = 0;
static int nx = 0, ny = 0, nz = 0;
static long lattice_size = 0;

static float xlo = 0.0f, ylo = 0.0f, zlo = 0.0f;
static float inv_gridspacing = 0.0f;
static float a2 = 0.0f, inv_a2 = 0.0f;
static int radius = 0;

static float *atoms = nullptr;        /* [natoms][4] flat: x, y, z, q */
static float *lattice = nullptr;      /* [nz][ny][nx] flat */
static float *lattice_ref = nullptr;


static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "r");
    if (fp) return fp;

    char buf[2048];
    snprintf(buf, sizeof(buf),
             "vendor/parboil/datasets/cutcp/small/input/%s", path);
    fp = fopen(buf, "r");
    return fp;
}


#define CUTCP_LINELEN 96

static bool read_atom_file(const char *path, std::vector<float> &out) {
    FILE *file = open_resolved(path);
    if (!file) return false;

    char line[CUTCP_LINELEN];
    out.clear();
    while (fgets(line, CUTCP_LINELEN, file) != nullptr) {
        if (strncmp(line, "ATOM  ", 6) != 0 && strncmp(line, "HETATM", 6) != 0)
            continue;  /* skip anything that isn't an atom record */

        float x, y, z, q;
        if (sscanf(line, "%*s %*d %*s %*s %*d %f %f %f %f", &x, &y, &z, &q) != 4) {
            fprintf(stderr, "[cutcp] atom record %zu does not have expected format\n",
                    out.size() / 4 + 1);
            fclose(file);
            return false;
        }
        out.push_back(x);
        out.push_back(y);
        out.push_back(z);
        out.push_back(q);
    }
    fclose(file);
    return !out.empty();
}

static void synth_atoms(std::vector<float> &out, int nmolecules) {
    srand(7);
    out.clear();
    out.reserve((size_t)nmolecules * 3 * 4);
    for (int m = 0; m < nmolecules; m++) {
        float ox = (float)(rand() % 40000) / 1000.0f - 20.0f;
        float oy = (float)(rand() % 40000) / 1000.0f - 20.0f;
        float oz = (float)(rand() % 40000) / 1000.0f - 20.0f;
        out.push_back(ox); out.push_back(oy); out.push_back(oz);
        out.push_back(-0.834f);
        for (int hcount = 0; hcount < 2; hcount++) {
            float dx = (float)(rand() % 2000 - 1000) / 1000.0f;
            float dy = (float)(rand() % 2000 - 1000) / 1000.0f;
            float dz = (float)(rand() % 2000 - 1000) / 1000.0f;
            out.push_back(ox + dx);
            out.push_back(oy + dy);
            out.push_back(oz + dz);
            out.push_back(0.417f);
        }
    }
}

static void bench_alloc(void) {
    const char *e;
    if ((e = getenv("CUTCP_SPACING"))) {
        float v = (float)atof(e);
        if (v > 0.0f) gridspacing = v;
    }
    if ((e = getenv("CUTCP_CUTOFF"))) {
        float v = (float)atof(e);
        if (v > 0.0f) cutoff = v;
    }

    const char *input = getenv("CUTCP_INPUT");
    if (!input) input = "vendor/parboil/datasets/cutcp/small/input/watbox.sl40.pqr";

    std::vector<float> raw;
    bool real_data = read_atom_file(input, raw);
    if (!real_data) {
        fprintf(stderr, "[cutcp] dataset %s unavailable, using synthetic water box\n",
                input);
        synth_atoms(raw, 1981);  
    }

    natoms = (int)(raw.size() / 4);

    if ((e = getenv("CUTCP_NATOMS"))) {
        int v = atoi(e);
        if (v > 0 && v < natoms) natoms = v;
    }

    atoms = dhir_alloc((long)natoms * 4);
    memcpy(atoms, raw.data(), (size_t)natoms * 4 * sizeof(float));

    float lox = atoms[0], hix = atoms[0];
    float loy = atoms[1], hiy = atoms[1];
    float loz = atoms[2], hiz = atoms[2];
    for (int n = 1; n < natoms; n++) {
        lox = fminf(lox, atoms[n * 4 + 0]);
        hix = fmaxf(hix, atoms[n * 4 + 0]);
        loy = fminf(loy, atoms[n * 4 + 1]);
        hiy = fmaxf(hiy, atoms[n * 4 + 1]);
        loz = fminf(loz, atoms[n * 4 + 2]);
        hiz = fmaxf(hiz, atoms[n * 4 + 2]);
    }

    xlo = lox - padding;
    ylo = loy - padding;
    zlo = loz - padding;
    float xhi = hix + padding;
    float yhi = hiy + padding;
    float zhi = hiz + padding;

    nx = (int)floorf((xhi - xlo) / gridspacing) + 1;
    ny = (int)floorf((yhi - ylo) / gridspacing) + 1;
    nz = (int)floorf((zhi - zlo) / gridspacing) + 1;
    lattice_size = (long)nx * ny * nz;

    a2 = cutoff * cutoff;
    inv_a2 = 1.f / a2;
    inv_gridspacing = 1.f / gridspacing;
    radius = (int)ceilf(cutoff * inv_gridspacing) - 1;

    lattice = dhir_alloc(lattice_size);
    lattice_ref = dhir_alloc(lattice_size);
    dhir_zero(lattice, lattice_size);       
    dhir_zero(lattice_ref, lattice_size);

    printf("[cutcp] %s natoms=%d lattice=%dx%dx%d (%ld) h=%g cutoff=%g radius=%d\n",
           real_data ? "dataset" : "synthetic", natoms, nx, ny, nz, lattice_size,
           (double)gridspacing, (double)cutoff, radius);
}

static void bench_call(void) {
    cutcp(natoms,
          MR2(atoms, natoms, 4),
          nx, ny, nz,
          MR3(lattice, nz, ny, nx),
          xlo, ylo, zlo,
          gridspacing, inv_gridspacing, radius,
          a2, inv_a2);
}

static void bench_reference(void) {
    ref_cutcp(natoms, atoms, nx, ny, nz, lattice_ref,
              xlo, ylo, zlo, gridspacing, inv_gridspacing, radius,
              a2, inv_a2);
}

static int bench_check(void) {
    return dhir_compare("lattice", lattice, lattice_ref, lattice_size, DHIR_EPS);
}

static void bench_free(void) {
    free(atoms);
    free(lattice);
    free(lattice_ref);
    atoms = lattice = lattice_ref = nullptr;
}
