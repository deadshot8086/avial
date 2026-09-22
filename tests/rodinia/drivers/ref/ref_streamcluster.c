#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SP 1
#define ITER 3
#define INT_MAX 2147483647

#ifdef __cplusplus
extern "C" {
#endif

void ref_streamcluster(
    int num, int dim, int kmin, int kmax,
    float *coord,
    float *weight,
    int *assign,
    float *point_cost,
    int *is_center,
    int *center_table,
    int *switch_membership,
    float *lower,
    float *gl_lower,
    int *feasible
) {
    float (*c)[dim] = (float (*)[dim])coord;

    // 1. Initial facility cost hiz
    double hiz = 0.0;
    for (int kk = 0; kk < num; kk++) {
        float d2 = 0.0f;
        for (int d = 0; d < dim; d++) {
            float diff = c[kk][d] - c[0][d];
            d2 += diff * diff;
        }
        hiz += (double)(d2 * weight[kk]);
    }
    double loz = 0.0;
    double z = (hiz + loz) / 2.0;

    // 2. Shuffle points
    for (int i = 0; i < num - 1; i++) {
        int j = (int)(lrand48() % (num - i)) + i;
        float tw = weight[i]; weight[i] = weight[j]; weight[j] = tw;
        for (int d = 0; d < dim; d++) {
            float tc = c[i][d]; c[i][d] = c[j][d]; c[j][d] = tc;
        }
    }

    // 3. Initial pspeedy
    long k = 1;
    for (int kk = 0; kk < num; kk++) {
        float d2 = 0.0f;
        for (int d = 0; d < dim; d++) {
            float diff = c[kk][d] - c[0][d];
            d2 += diff * diff;
        }
        point_cost[kk] = d2 * weight[kk];
        assign[kk] = 0;
    }
    for (int i = 1; i < num; i++) {
        int to_open = (((float)lrand48() / (float)INT_MAX) < (point_cost[i] / (float)z));
        if (to_open) {
            k++;
            for (int kk = 0; kk < num; kk++) {
                float d2 = 0.0f;
                for (int d = 0; d < dim; d++) {
                    float diff = c[i][d] - c[kk][d];
                    d2 += diff * diff;
                }
                if (d2 * weight[kk] < point_cost[kk]) {
                    point_cost[kk] = d2 * weight[kk];
                    assign[kk] = i;
                }
            }
        }
    }

    // 4. Speedy retry while k < kmin
    int sp_i = 0;
    while ((k < kmin) && (sp_i < SP)) {
        for (int kk = 0; kk < num; kk++) {
            float d2 = 0.0f;
            for (int d = 0; d < dim; d++) {
                float diff = c[kk][d] - c[0][d];
                d2 += diff * diff;
            }
            point_cost[kk] = d2 * weight[kk];
            assign[kk] = 0;
        }
        k = 1;
        for (int i = 1; i < num; i++) {
            int to_open = (((float)lrand48() / (float)INT_MAX) < (point_cost[i] / (float)z));
            if (to_open) {
                k++;
                for (int kk = 0; kk < num; kk++) {
                    float d2 = 0.0f;
                    for (int d = 0; d < dim; d++) {
                        float diff = c[i][d] - c[kk][d];
                        d2 += diff * diff;
                    }
                    if (d2 * weight[kk] < point_cost[kk]) {
                        point_cost[kk] = d2 * weight[kk];
                        assign[kk] = i;
                    }
                }
            }
        }
        sp_i++;
    }

    while (k < kmin) {
        if (sp_i >= SP) {
            hiz = z;
            z = (hiz + loz) / 2.0;
            sp_i = 0;
        }
        for (int i = 0; i < num - 1; i++) {
            int j = (int)(lrand48() % (num - i)) + i;
            float tw = weight[i]; weight[i] = weight[j]; weight[j] = tw;
            for (int d = 0; d < dim; d++) {
                float tc = c[i][d]; c[i][d] = c[j][d]; c[j][d] = tc;
            }
        }
        for (int kk = 0; kk < num; kk++) {
            float d2 = 0.0f;
            for (int d = 0; d < dim; d++) {
                float diff = c[kk][d] - c[0][d];
                d2 += diff * diff;
            }
            point_cost[kk] = d2 * weight[kk];
            assign[kk] = 0;
        }
        k = 1;
        for (int i = 1; i < num; i++) {
            int to_open = (((float)lrand48() / (float)INT_MAX) < (point_cost[i] / (float)z));
            if (to_open) {
                k++;
                for (int kk = 0; kk < num; kk++) {
                    float d2 = 0.0f;
                    for (int d = 0; d < dim; d++) {
                        float diff = c[i][d] - c[kk][d];
                        d2 += diff * diff;
                    }
                    if (d2 * weight[kk] < point_cost[kk]) {
                        point_cost[kk] = d2 * weight[kk];
                        assign[kk] = i;
                    }
                }
            }
        }
        sp_i++;
    }

    // 5. selectfeasible_fast
    int numfeasible = num;
    int max_feasible = (int)(ITER * kmin * log((double)kmin));
    if (numfeasible > max_feasible)
        numfeasible = max_feasible;

    if (numfeasible == num) {
        for (int i = 0; i < numfeasible; i++) feasible[i] = i;
    } else {
        lower[0] = weight[0];
        for (int i = 1; i < num; i++) lower[i] = lower[i - 1] + weight[i];
        float totalweight = lower[num - 1];

        for (int i = 0; i < numfeasible; i++) {
            float w = ((float)lrand48() / (float)INT_MAX) * totalweight;
            int l = 0, r = num - 1;
            if (lower[0] > w) {
                feasible[i] = 0;
            } else {
                while (l + 1 < r) {
                    int mid = (l + r) / 2;
                    if (lower[mid] > w) r = mid;
                    else l = mid;
                }
                feasible[i] = r;
            }
        }
    }

    for (int i = 0; i < num; i++) is_center[i] = 0;
    for (int i = 0; i < num; i++) is_center[assign[i]] = 1;

    double cost = 0.0;
    for (int i = 0; i < num; i++) cost += (double)point_cost[i];
    cost += z * (double)k;

    // 6. Local search loop
    while (1) {
        for (int stage = 0; stage < 2; stage++) {
            if (stage == 1) {
                if (!(((k <= (int)(1.1 * kmax)) && (k >= (int)(0.9 * kmin))) ||
                      ((k <= kmax + 2) && (k >= kmin - 2)))) {
                    break;
                }
            }
            double eps = (stage == 0) ? 0.1 : 0.001;
            double change = cost;
            long iter = (long)(ITER * kmax * log((double)kmax));
            while (change / cost > eps) {
                change = 0.0;
                for (int i = 0; i < numfeasible; i++) {
                    int j = (int)(lrand48() % (numfeasible - i)) + i;
                    int t = feasible[i];
                    feasible[i] = feasible[j];
                    feasible[j] = t;
                }

                long iter = (long)(ITER * kmax * log((double)kmax));
                for (long it = 0; it < iter; it++) {
                    int x = feasible[it % numfeasible];

                    int count = 0;
                    for (int i = 0; i < num; i++) {
                        if (is_center[i]) {
                            center_table[i] = count++;
                        }
                    }

                    for (int i = 0; i < count; i++) lower[i] = 0.0f;
                    for (int i = 0; i < num; i++) switch_membership[i] = 0;

                    double cost_of_opening_x = 0.0;
                    for (int i = 0; i < num; i++) {
                        float d2 = 0.0f;
                        for (int d = 0; d < dim; d++) {
                            float diff = c[i][d] - c[x][d];
                            d2 += diff * diff;
                        }
                        float x_cost = d2 * weight[i];
                        float current_c = point_cost[i];
                        if (x_cost < current_c) {
                            switch_membership[i] = 1;
                            cost_of_opening_x += (double)(x_cost - current_c);
                        } else {
                            int a = assign[i];
                            int c_idx = center_table[a];
                            lower[c_idx] += (current_c - x_cost);
                        }
                    }

                    int number_of_centers_to_close = 0;
                    for (int i = 0; i < num; i++) {
                        if (is_center[i]) {
                            int c_idx = center_table[i];
                            double low = z + (double)lower[c_idx];
                            gl_lower[c_idx] = (float)low;
                            if (low > 0.0) {
                                number_of_centers_to_close++;
                                cost_of_opening_x -= low;
                            }
                        }
                    }

                    cost_of_opening_x += z;

                    if (cost_of_opening_x < 0.0) {
                        for (int i = 0; i < num; i++) {
                            int close_center = (gl_lower[center_table[assign[i]]] > 0.0f);
                            if (switch_membership[i] || close_center) {
                                float d2 = 0.0f;
                                for (int d = 0; d < dim; d++) {
                                    float diff = c[i][d] - c[x][d];
                                    d2 += diff * diff;
                                }
                                point_cost[i] = d2 * weight[i];
                                assign[i] = x;
                            }
                        }

                        for (int i = 0; i < num; i++) {
                            if (is_center[i] && (gl_lower[center_table[i]] > 0.0f)) {
                                is_center[i] = 0;
                            }
                        }
                        is_center[x] = 1;
                        k = k + 1 - number_of_centers_to_close;
                        change += (-cost_of_opening_x);
                    }
                }
                cost -= change;
            }
        }

        if (k > kmax) {
            loz = z; z = (hiz + loz) / 2.0;
            cost += (z - loz) * (double)k;
        }
        if (k < kmin) {
            hiz = z; z = (hiz + loz) / 2.0;
            cost += (z - hiz) * (double)k;
        }
        if (((k <= kmax) && (k >= kmin)) || (loz >= 0.999 * hiz)) {
            break;
        }
    }

    // 7. contcenters
    for (int i = 0; i < num; i++) {
        int a = assign[i];
        if (a != i) {
            float relweight = weight[a] + weight[i];
            relweight = weight[i] / relweight;
            for (int d = 0; d < dim; d++) {
                c[a][d] *= 1.0 - relweight;
                c[a][d] += c[i][d] * relweight;
            }
            weight[a] += weight[i];
        }
    }

    // 8. Final is_center marking
    for (int i = 0; i < num; i++) is_center[i] = 0;
    for (int i = 0; i < num; i++) is_center[assign[i]] = 1;
}

#ifdef __cplusplus
}
#endif
