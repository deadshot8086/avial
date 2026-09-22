// Rodinia CFD (Euler3D) Benchmark Reference Implementation
// Matches official Rodinia 3.1 openmp/cfd/euler3d_cpu.cpp
// Canonical multi-dimensional representation without pointers/structs.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GAMMA 1.4f
#define NDIM 3
#define NNB 4
#define RK 3

#define VAR_DENSITY 0
#define VAR_MOMENTUM 1
#define VAR_DENSITY_ENERGY (VAR_MOMENTUM + NDIM)
#define NVAR (VAR_DENSITY_ENERGY + 1)

#define ITERATIONS 1000

#ifdef __cplusplus
extern "C" {
#endif

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
             const float ff_flux_contribution_momentum_z[NDIM]) {
    const float smoothing_coefficient = 0.2f;

    for (int iter = 0; iter < ITERATIONS; iter++) {
        // 1. Copy variables to old_variables
        for (int i = 0; i < nelr; i++) {
            for (int v = 0; v < NVAR; v++) {
                old_variables[i][v] = variables[i][v];
            }
        }

    // 2. Compute step factor
    for (int i = 0; i < nelr; i++) {
        float density = variables[i][VAR_DENSITY];
        float momentum_x = variables[i][VAR_MOMENTUM + 0];
        float momentum_y = variables[i][VAR_MOMENTUM + 1];
        float momentum_z = variables[i][VAR_MOMENTUM + 2];
        float density_energy = variables[i][VAR_DENSITY_ENERGY];

        float velocity_x = momentum_x / density;
        float velocity_y = momentum_y / density;
        float velocity_z = momentum_z / density;

        float speed_sqd = velocity_x * velocity_x + velocity_y * velocity_y + velocity_z * velocity_z;
        float pressure = (GAMMA - 1.0f) * (density_energy - 0.5f * density * speed_sqd);
        float speed_of_sound = sqrtf(GAMMA * pressure / density);

        step_factors[i] = 0.5f / (sqrtf(areas[i]) * (sqrtf(speed_sqd) + speed_of_sound));
    }

    // 3. 3-stage Runge-Kutta time stepping
    for (int j = 0; j < RK; j++) {
        // Compute flux
        for (int i = 0; i < nelr; i++) {
            float density_i = variables[i][VAR_DENSITY];
            float momentum_i_x = variables[i][VAR_MOMENTUM + 0];
            float momentum_i_y = variables[i][VAR_MOMENTUM + 1];
            float momentum_i_z = variables[i][VAR_MOMENTUM + 2];
            float density_energy_i = variables[i][VAR_DENSITY_ENERGY];

            float velocity_i_x = momentum_i_x / density_i;
            float velocity_i_y = momentum_i_y / density_i;
            float velocity_i_z = momentum_i_z / density_i;

            float speed_sqd_i = velocity_i_x * velocity_i_x + velocity_i_y * velocity_i_y + velocity_i_z * velocity_i_z;
            float speed_i = sqrtf(speed_sqd_i);
            float pressure_i = (GAMMA - 1.0f) * (density_energy_i - 0.5f * density_i * speed_sqd_i);
            float speed_of_sound_i = sqrtf(GAMMA * pressure_i / density_i);

            float fc_i_momentum_x_x = velocity_i_x * momentum_i_x + pressure_i;
            float fc_i_momentum_x_y = velocity_i_x * momentum_i_y;
            float fc_i_momentum_x_z = velocity_i_x * momentum_i_z;

            float fc_i_momentum_y_x = fc_i_momentum_x_y;
            float fc_i_momentum_y_y = velocity_i_y * momentum_i_y + pressure_i;
            float fc_i_momentum_y_z = velocity_i_y * momentum_i_z;

            float fc_i_momentum_z_x = fc_i_momentum_x_z;
            float fc_i_momentum_z_y = fc_i_momentum_y_z;
            float fc_i_momentum_z_z = velocity_i_z * momentum_i_z + pressure_i;

            float de_p_i = density_energy_i + pressure_i;
            float fc_i_density_energy_x = velocity_i_x * de_p_i;
            float fc_i_density_energy_y = velocity_i_y * de_p_i;
            float fc_i_density_energy_z = velocity_i_z * de_p_i;

            float flux_i_density = 0.0f;
            float flux_i_momentum_x = 0.0f;
            float flux_i_momentum_y = 0.0f;
            float flux_i_momentum_z = 0.0f;
            float flux_i_density_energy = 0.0f;

            for (int nb_idx = 0; nb_idx < NNB; nb_idx++) {
                int nb = elements_surrounding_elements[i][nb_idx];
                float normal_x = normals[i][nb_idx][0];
                float normal_y = normals[i][nb_idx][1];
                float normal_z = normals[i][nb_idx][2];
                float normal_len = sqrtf(normal_x * normal_x + normal_y * normal_y + normal_z * normal_z);

                if (nb >= 0) {
                    float density_nb = variables[nb][VAR_DENSITY];
                    float momentum_nb_x = variables[nb][VAR_MOMENTUM + 0];
                    float momentum_nb_y = variables[nb][VAR_MOMENTUM + 1];
                    float momentum_nb_z = variables[nb][VAR_MOMENTUM + 2];
                    float density_energy_nb = variables[nb][VAR_DENSITY_ENERGY];

                    float velocity_nb_x = momentum_nb_x / density_nb;
                    float velocity_nb_y = momentum_nb_y / density_nb;
                    float velocity_nb_z = momentum_nb_z / density_nb;

                    float speed_sqd_nb = velocity_nb_x * velocity_nb_x + velocity_nb_y * velocity_nb_y + velocity_nb_z * velocity_nb_z;
                    float pressure_nb = (GAMMA - 1.0f) * (density_energy_nb - 0.5f * density_nb * speed_sqd_nb);
                    float speed_of_sound_nb = sqrtf(GAMMA * pressure_nb / density_nb);

                    float fc_nb_momentum_x_x = velocity_nb_x * momentum_nb_x + pressure_nb;
                    float fc_nb_momentum_x_y = velocity_nb_x * momentum_nb_y;
                    float fc_nb_momentum_x_z = velocity_nb_x * momentum_nb_z;

                    float fc_nb_momentum_y_x = fc_nb_momentum_x_y;
                    float fc_nb_momentum_y_y = velocity_nb_y * momentum_nb_y + pressure_nb;
                    float fc_nb_momentum_y_z = velocity_nb_y * momentum_nb_z;

                    float fc_nb_momentum_z_x = fc_nb_momentum_x_z;
                    float fc_nb_momentum_z_y = fc_nb_momentum_y_z;
                    float fc_nb_momentum_z_z = velocity_nb_z * momentum_nb_z + pressure_nb;

                    float de_p_nb = density_energy_nb + pressure_nb;
                    float fc_nb_density_energy_x = velocity_nb_x * de_p_nb;
                    float fc_nb_density_energy_y = velocity_nb_y * de_p_nb;
                    float fc_nb_density_energy_z = velocity_nb_z * de_p_nb;

                    // Artificial viscosity
                    float factor = -normal_len * smoothing_coefficient * 0.5f * (speed_i + sqrtf(speed_sqd_nb) + speed_of_sound_i + speed_of_sound_nb);
                    flux_i_density += factor * (density_i - density_nb);
                    flux_i_density_energy += factor * (density_energy_i - density_energy_nb);
                    flux_i_momentum_x += factor * (momentum_i_x - momentum_nb_x);
                    flux_i_momentum_y += factor * (momentum_i_y - momentum_nb_y);
                    flux_i_momentum_z += factor * (momentum_i_z - momentum_nb_z);

                    // Cell-centered flux accumulation
                    factor = 0.5f * normal_x;
                    flux_i_density += factor * (momentum_nb_x + momentum_i_x);
                    flux_i_density_energy += factor * (fc_nb_density_energy_x + fc_i_density_energy_x);
                    flux_i_momentum_x += factor * (fc_nb_momentum_x_x + fc_i_momentum_x_x);
                    flux_i_momentum_y += factor * (fc_nb_momentum_y_x + fc_i_momentum_y_x);
                    flux_i_momentum_z += factor * (fc_nb_momentum_z_x + fc_i_momentum_z_x);

                    factor = 0.5f * normal_y;
                    flux_i_density += factor * (momentum_nb_y + momentum_i_y);
                    flux_i_density_energy += factor * (fc_nb_density_energy_y + fc_i_density_energy_y);
                    flux_i_momentum_x += factor * (fc_nb_momentum_x_y + fc_i_momentum_x_y);
                    flux_i_momentum_y += factor * (fc_nb_momentum_y_y + fc_i_momentum_y_y);
                    flux_i_momentum_z += factor * (fc_nb_momentum_z_y + fc_i_momentum_z_y);

                    factor = 0.5f * normal_z;
                    flux_i_density += factor * (momentum_nb_z + momentum_i_z);
                    flux_i_density_energy += factor * (fc_nb_density_energy_z + fc_i_density_energy_z);
                    flux_i_momentum_x += factor * (fc_nb_momentum_x_z + fc_i_momentum_x_z);
                    flux_i_momentum_y += factor * (fc_nb_momentum_y_z + fc_i_momentum_y_z);
                    flux_i_momentum_z += factor * (fc_nb_momentum_z_z + fc_i_momentum_z_z);
                } else if (nb == -1) {
                    // Wing boundary
                    flux_i_momentum_x += normal_x * pressure_i;
                    flux_i_momentum_y += normal_y * pressure_i;
                    flux_i_momentum_z += normal_z * pressure_i;
                } else if (nb == -2) {
                    // Far field boundary
                    float factor = 0.5f * normal_x;
                    flux_i_density += factor * (ff_variable[VAR_MOMENTUM + 0] + momentum_i_x);
                    flux_i_density_energy += factor * (ff_flux_contribution_density_energy[0] + fc_i_density_energy_x);
                    flux_i_momentum_x += factor * (ff_flux_contribution_momentum_x[0] + fc_i_momentum_x_x);
                    flux_i_momentum_y += factor * (ff_flux_contribution_momentum_y[0] + fc_i_momentum_y_x);
                    flux_i_momentum_z += factor * (ff_flux_contribution_momentum_z[0] + fc_i_momentum_z_x);

                    factor = 0.5f * normal_y;
                    flux_i_density += factor * (ff_variable[VAR_MOMENTUM + 1] + momentum_i_y);
                    flux_i_density_energy += factor * (ff_flux_contribution_density_energy[1] + fc_i_density_energy_y);
                    flux_i_momentum_x += factor * (ff_flux_contribution_momentum_x[1] + fc_i_momentum_x_y);
                    flux_i_momentum_y += factor * (ff_flux_contribution_momentum_y[1] + fc_i_momentum_y_y);
                    flux_i_momentum_z += factor * (ff_flux_contribution_momentum_z[1] + fc_i_momentum_z_y);

                    factor = 0.5f * normal_z;
                    flux_i_density += factor * (ff_variable[VAR_MOMENTUM + 2] + momentum_i_z);
                    flux_i_density_energy += factor * (ff_flux_contribution_density_energy[2] + fc_i_density_energy_z);
                    flux_i_momentum_x += factor * (ff_flux_contribution_momentum_x[2] + fc_i_momentum_x_z);
                    flux_i_momentum_y += factor * (ff_flux_contribution_momentum_y[2] + fc_i_momentum_y_z);
                    flux_i_momentum_z += factor * (ff_flux_contribution_momentum_z[2] + fc_i_momentum_z_z);
                }
            }

            fluxes[i][VAR_DENSITY] = flux_i_density;
            fluxes[i][VAR_MOMENTUM + 0] = flux_i_momentum_x;
            fluxes[i][VAR_MOMENTUM + 1] = flux_i_momentum_y;
            fluxes[i][VAR_MOMENTUM + 2] = flux_i_momentum_z;
            fluxes[i][VAR_DENSITY_ENERGY] = flux_i_density_energy;
        }

        // Time step
        for (int i = 0; i < nelr; i++) {
            float factor = step_factors[i] / (float)(RK + 1 - j);
            variables[i][VAR_DENSITY] = old_variables[i][VAR_DENSITY] + factor * fluxes[i][VAR_DENSITY];
            variables[i][VAR_MOMENTUM + 0] = old_variables[i][VAR_MOMENTUM + 0] + factor * fluxes[i][VAR_MOMENTUM + 0];
            variables[i][VAR_MOMENTUM + 1] = old_variables[i][VAR_MOMENTUM + 1] + factor * fluxes[i][VAR_MOMENTUM + 1];
            variables[i][VAR_MOMENTUM + 2] = old_variables[i][VAR_MOMENTUM + 2] + factor * fluxes[i][VAR_MOMENTUM + 2];
            variables[i][VAR_DENSITY_ENERGY] = old_variables[i][VAR_DENSITY_ENERGY] + factor * fluxes[i][VAR_DENSITY_ENERGY];
        }
    }
    }
}

#ifdef __cplusplus
}
#endif
