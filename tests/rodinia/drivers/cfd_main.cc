#define DHIR_BENCH_NAME "cfd"
#include "dhir_bench.h"

#define GAMMA 1.4f
#define NDIM 3
#define NNB 4
#define RK 3

#define VAR_DENSITY 0
#define VAR_MOMENTUM 1
#define VAR_DENSITY_ENERGY (VAR_MOMENTUM + NDIM)
#define NVAR (VAR_DENSITY_ENERGY + 1)

static const int block_length = 8;
static const float ff_mach = 1.2f;
static const float deg_angle_of_attack = 0.0f;

extern "C" {
void cfd(int32_t,
         int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t,
         float *, float *, int64_t, int64_t, int64_t);

void ref_cfd(int nelr,
             const int elements_surrounding_elements[][NNB],
             const float normals[][NNB][NDIM],
             const float areas[],
             float old_variables[][NVAR],
             float variables[][NVAR],
             float step_factors[],
             float fluxes[][NVAR],
             const float ff_variable[NVAR],
             const float ff_flux_contribution_density_energy[NDIM],
             const float ff_flux_contribution_momentum_x[NDIM],
             const float ff_flux_contribution_momentum_y[NDIM],
             const float ff_flux_contribution_momentum_z[NDIM]);
}

static int nel = 0;
static int nelr = 0;
static long total_vars = 0;

static float *areas = nullptr;
static int (*elements_surrounding_elements)[NNB] = nullptr;
static float (*normals)[NNB][NDIM] = nullptr;

static float (*old_variables)[NVAR] = nullptr;
static float (*old_variables_ref)[NVAR] = nullptr;
static float (*variables)[NVAR] = nullptr;
static float (*variables_ref)[NVAR] = nullptr;
static float *step_factors = nullptr;
static float *step_factors_ref = nullptr;
static float (*fluxes)[NVAR] = nullptr;
static float (*fluxes_ref)[NVAR] = nullptr;

static float ff_variable[NVAR];
static float ff_flux_contribution_density_energy[NDIM];
static float ff_flux_contribution_momentum_x[NDIM];
static float ff_flux_contribution_momentum_y[NDIM];
static float ff_flux_contribution_momentum_z[NDIM];

static void bench_alloc(void) {
    const char *data_file = "vendor/rodinia/rodinia_3.1/data/cfd/fvcorr.domn.097K";
    FILE *fp = fopen(data_file, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open mesh dataset: %s\n", data_file);
        exit(1);
    }
    if (fscanf(fp, "%d", &nel) != 1) {
        fprintf(stderr, "Error reading nel from %s\n", data_file);
        exit(1);
    }

    nelr = block_length * ((nel / block_length) + ((nel % block_length != 0) ? 1 : 0));
    total_vars = (long)nelr * NVAR;

    // Set far-field boundary conditions matching Rodinia 3.1
    const float angle_of_attack = (float)(3.1415926535897931 / 180.0) * deg_angle_of_attack;
    ff_variable[VAR_DENSITY] = 1.4f;

    float ff_pressure = 1.0f;
    float ff_speed_of_sound = sqrtf(GAMMA * ff_pressure / ff_variable[VAR_DENSITY]);
    float ff_speed = ff_mach * ff_speed_of_sound;

    float ff_velocity[NDIM];
    ff_velocity[0] = ff_speed * cosf(angle_of_attack);
    ff_velocity[1] = ff_speed * sinf(angle_of_attack);
    ff_velocity[2] = 0.0f;

    ff_variable[VAR_MOMENTUM + 0] = ff_variable[VAR_DENSITY] * ff_velocity[0];
    ff_variable[VAR_MOMENTUM + 1] = ff_variable[VAR_DENSITY] * ff_velocity[1];
    ff_variable[VAR_MOMENTUM + 2] = ff_variable[VAR_DENSITY] * ff_velocity[2];

    ff_variable[VAR_DENSITY_ENERGY] = ff_variable[VAR_DENSITY] * (0.5f * (ff_speed * ff_speed)) + (ff_pressure / (GAMMA - 1.0f));

    float de_p = ff_variable[VAR_DENSITY_ENERGY] + ff_pressure;
    ff_flux_contribution_density_energy[0] = ff_velocity[0] * de_p;
    ff_flux_contribution_density_energy[1] = ff_velocity[1] * de_p;
    ff_flux_contribution_density_energy[2] = ff_velocity[2] * de_p;

    ff_flux_contribution_momentum_x[0] = ff_velocity[0] * ff_variable[VAR_MOMENTUM + 0] + ff_pressure;
    ff_flux_contribution_momentum_x[1] = ff_velocity[0] * ff_variable[VAR_MOMENTUM + 1];
    ff_flux_contribution_momentum_x[2] = ff_velocity[0] * ff_variable[VAR_MOMENTUM + 2];

    ff_flux_contribution_momentum_y[0] = ff_flux_contribution_momentum_x[1];
    ff_flux_contribution_momentum_y[1] = ff_velocity[1] * ff_variable[VAR_MOMENTUM + 1] + ff_pressure;
    ff_flux_contribution_momentum_y[2] = ff_velocity[1] * ff_variable[VAR_MOMENTUM + 2];

    ff_flux_contribution_momentum_z[0] = ff_flux_contribution_momentum_x[2];
    ff_flux_contribution_momentum_z[1] = ff_flux_contribution_momentum_y[2];
    ff_flux_contribution_momentum_z[2] = ff_velocity[2] * ff_variable[VAR_MOMENTUM + 2] + ff_pressure;

    // Allocate memory
    areas = dhir_alloc(nelr);
    elements_surrounding_elements = (int (*)[NNB])dhir_alloc_int(nelr * NNB);
    normals = (float (*)[NNB][NDIM])dhir_alloc(nelr * NNB * NDIM);

    old_variables = (float (*)[NVAR])dhir_alloc(total_vars);
    old_variables_ref = (float (*)[NVAR])dhir_alloc(total_vars);
    variables = (float (*)[NVAR])dhir_alloc(total_vars);
    variables_ref = (float (*)[NVAR])dhir_alloc(total_vars);
    step_factors = dhir_alloc(nelr);
    step_factors_ref = dhir_alloc(nelr);
    fluxes = (float (*)[NVAR])dhir_alloc(total_vars);
    fluxes_ref = (float (*)[NVAR])dhir_alloc(total_vars);

    // Read mesh geometry from dataset file
    for (int i = 0; i < nel; i++) {
        if (fscanf(fp, "%f", &areas[i]) != 1) {
            fprintf(stderr, "Failed reading area for element %d\n", i);
            exit(1);
        }
        for (int j = 0; j < NNB; j++) {
            int nb;
            if (fscanf(fp, "%d", &nb) != 1) {
                fprintf(stderr, "Failed reading neighbor %d for element %d\n", j, i);
                exit(1);
            }
            if (nb < 0) nb = -1;
            nb--; // Fortran 1-based indexing
            elements_surrounding_elements[i][j] = nb;

            for (int k = 0; k < NDIM; k++) {
                float n;
                if (fscanf(fp, "%f", &n) != 1) {
                    fprintf(stderr, "Failed reading normal %d,%d for element %d\n", j, k, i);
                    exit(1);
                }
                normals[i][j][k] = -n;
            }
        }
    }
    fclose(fp);

    // Pad remaining elements up to nelr by duplicating the last element
    int last = nel - 1;
    for (int i = nel; i < nelr; i++) {
        areas[i] = areas[last];
        for (int j = 0; j < NNB; j++) {
            elements_surrounding_elements[i][j] = elements_surrounding_elements[last][j];
            for (int k = 0; k < NDIM; k++) {
                normals[i][j][k] = normals[last][j][k];
            }
        }
    }

    // Initialize variables to far-field conditions
    for (int i = 0; i < nelr; i++) {
        for (int j = 0; j < NVAR; j++) {
            variables[i][j] = ff_variable[j];
            variables_ref[i][j] = ff_variable[j];
        }
    }
}

static void bench_call(void) {
    cfd(nelr,
        MR2((int *)elements_surrounding_elements, nelr, NNB),
        MR3((float *)normals, nelr, NNB, NDIM),
        MR1(areas, nelr),
        MR2((float *)old_variables, nelr, NVAR),
        MR2((float *)variables, nelr, NVAR),
        MR1(step_factors, nelr),
        MR2((float *)fluxes, nelr, NVAR),
        MR1(ff_variable, NVAR),
        MR1(ff_flux_contribution_density_energy, NDIM),
        MR1(ff_flux_contribution_momentum_x, NDIM),
        MR1(ff_flux_contribution_momentum_y, NDIM),
        MR1(ff_flux_contribution_momentum_z, NDIM));
}

static void bench_reference(void) {
    ref_cfd(nelr,
            elements_surrounding_elements,
            normals,
            areas,
            old_variables_ref,
            variables_ref,
            step_factors_ref,
            fluxes_ref,
            ff_variable,
            ff_flux_contribution_density_energy,
            ff_flux_contribution_momentum_x,
            ff_flux_contribution_momentum_y,
            ff_flux_contribution_momentum_z);
}

static int bench_check(void) {
    return dhir_compare("variables", (const float *)variables, (const float *)variables_ref, total_vars, DHIR_EPS);
}

static void bench_free(void) {
    free(areas);
    free(elements_surrounding_elements);
    free(normals);
    free(old_variables);
    free(old_variables_ref);
    free(variables);
    free(variables_ref);
    free(step_factors);
    free(step_factors_ref);
    free(fluxes);
    free(fluxes_ref);
}
