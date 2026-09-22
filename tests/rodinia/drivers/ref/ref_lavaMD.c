#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUMBER_PAR_PER_BOX 100

void ref_lavaMD(double alpha, int number_boxes, int max_neighbors,
                const int *box_nn, const int *box_nei,
                const double *rv, const double *qv, double *fv) {
    double a2 = 2.0 * alpha * alpha;

    for (int l = 0; l < number_boxes; l++) {
        int first_i = l * NUMBER_PAR_PER_BOX;
        int nn = box_nn[l];

        for (int k = 0; k < 1 + nn; k++) {
            int pointer = (k == 0) ? l : box_nei[l * max_neighbors + (k - 1)];
            int first_j = pointer * NUMBER_PAR_PER_BOX;

            for (int i = 0; i < NUMBER_PAR_PER_BOX; i++) {
                int idx_i = (first_i + i) * 4;
                double rA_v = rv[idx_i + 0];
                double rA_x = rv[idx_i + 1];
                double rA_y = rv[idx_i + 2];
                double rA_z = rv[idx_i + 3];

                double sum_v = 0.0;
                double sum_x = 0.0;
                double sum_y = 0.0;
                double sum_z = 0.0;

                for (int j = 0; j < NUMBER_PAR_PER_BOX; j++) {
                    int idx_j = (first_j + j) * 4;
                    double rB_v = rv[idx_j + 0];
                    double rB_x = rv[idx_j + 1];
                    double rB_y = rv[idx_j + 2];
                    double rB_z = rv[idx_j + 3];
                    double qB_val = qv[first_j + j];

                    double r2 = rA_v + rB_v - (rA_x * rB_x + rA_y * rB_y + rA_z * rB_z);
                    double u2 = a2 * r2;
                    double vij = exp(-u2);
                    double fs = 2.0 * vij;
                    double dx = rA_x - rB_x;
                    double dy = rA_y - rB_y;
                    double dz = rA_z - rB_z;

                    sum_v += qB_val * vij;
                    sum_x += qB_val * (fs * dx);
                    sum_y += qB_val * (fs * dy);
                    sum_z += qB_val * (fs * dz);
                }

                fv[idx_i + 0] += sum_v;
                fv[idx_i + 1] += sum_x;
                fv[idx_i + 2] += sum_y;
                fv[idx_i + 3] += sum_z;
            }
        }
    }
}

#ifdef __cplusplus
}
#endif
