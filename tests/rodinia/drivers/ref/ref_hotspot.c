#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_hotspot(int row, int col, int num_iterations,
                 const float *power,
                 float *temp,
                 float *result,
                 float Cap_1, float Rx_1, float Ry_1, float Rz_1, float amb_temp) {
    for (int iter = 0; iter < num_iterations; iter += 2) {
        /* Step A: read temp, write result */
        for (int r = 0; r < row; r++) {
            for (int c = 0; c < col; c++) {
                float t_c = temp[r * col + c];
                float t_n = (r > 0) ? temp[(r - 1) * col + c] : t_c;
                float t_s = (r < row - 1) ? temp[(r + 1) * col + c] : t_c;
                float t_w = (c > 0) ? temp[r * col + (c - 1)] : t_c;
                float t_e = (c < col - 1) ? temp[r * col + (c + 1)] : t_c;

                float delta = Cap_1 * (power[r * col + c] +
                    (t_s + t_n - 2.0f * t_c) * Ry_1 +
                    (t_e + t_w - 2.0f * t_c) * Rx_1 +
                    (amb_temp - t_c) * Rz_1);
                result[r * col + c] = t_c + delta;
            }
        }
        /* Step B: read result, write temp */
        if (iter + 1 < num_iterations) {
            for (int r = 0; r < row; r++) {
                for (int c = 0; c < col; c++) {
                    float t_c = result[r * col + c];
                    float t_n = (r > 0) ? result[(r - 1) * col + c] : t_c;
                    float t_s = (r < row - 1) ? result[(r + 1) * col + c] : t_c;
                    float t_w = (c > 0) ? result[r * col + (c - 1)] : t_c;
                    float t_e = (c < col - 1) ? result[r * col + (c + 1)] : t_c;

                    float delta = Cap_1 * (power[r * col + c] +
                        (t_s + t_n - 2.0f * t_c) * Ry_1 +
                        (t_e + t_w - 2.0f * t_c) * Rx_1 +
                        (amb_temp - t_c) * Rz_1);
                    temp[r * col + c] = t_c + delta;
                }
            }
        }
    }
    if (num_iterations % 2 == 0) {
        for (int r = 0; r < row; r++) {
            for (int c = 0; c < col; c++) {
                result[r * col + c] = temp[r * col + c];
            }
        }
    }
}

#ifdef __cplusplus
}
#endif
