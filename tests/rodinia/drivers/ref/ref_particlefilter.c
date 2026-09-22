#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.1415926535897932

#ifdef __cplusplus
extern "C" {
#endif

static inline double ref_roundDouble(double value) {
    int newValue = (int)(value);
    if (value - newValue < 0.5)
        return (double)newValue;
    else
        return (double)(newValue++);
}

void pf_strel_disk(int *disk, int radius) {
    int diameter = radius * 2 - 1;
    for (int x = 0; x < diameter; x++) {
        for (int y = 0; y < diameter; y++) {
            double distance = sqrt(pow((double)(x - radius + 1), 2) + pow((double)(y - radius + 1), 2));
            if (distance < radius)
                disk[x * diameter + y] = 1;
            else
                disk[x * diameter + y] = 0;
        }
    }
}

void pf_getneighbors(int *se, int numOnes, int *neighbors, int radius) {
    int neighY = 0;
    int center = radius - 1;
    int diameter = radius * 2 - 1;
    for (int x = 0; x < diameter; x++) {
        for (int y = 0; y < diameter; y++) {
            if (se[x * diameter + y]) {
                neighbors[neighY * 2] = (int)(y - center);
                neighbors[neighY * 2 + 1] = (int)(x - center);
                neighY++;
            }
        }
    }
}

static void pf_dilate_matrix(int *matrix, int posX, int posY, int posZ, int dimX, int dimY, int dimZ, int error) {
    int startX = (posX - error < 0) ? 0 : posX - error;
    int startY = (posY - error < 0) ? 0 : posY - error;
    int endX = (posX + error > dimX) ? dimX : posX + error;
    int endY = (posY + error > dimY) ? dimY : posY + error;
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            double distance = sqrt(pow((double)(x - posX), 2) + pow((double)(y - posY), 2));
            if (distance < error)
                matrix[x * dimY * dimZ + y * dimZ + posZ] = 1;
        }
    }
}

void pf_imdilate_disk(int *matrix, int dimX, int dimY, int dimZ, int error, int *newMatrix) {
    memset(newMatrix, 0, sizeof(int) * dimX * dimY * dimZ);
    for (int z = 0; z < dimZ; z++) {
        for (int x = 0; x < dimX; x++) {
            for (int y = 0; y < dimY; y++) {
                if (matrix[x * dimY * dimZ + y * dimZ + z] == 1) {
                    pf_dilate_matrix(newMatrix, x, y, z, dimX, dimY, dimZ, error);
                }
            }
        }
    }
}

static double pf_randu(int *seed, int index) {
    long M = 2147483647;
    int A = 1103515245;
    int C = 12345;
    int num = A * seed[index] + C;
    seed[index] = num % M;
    return fabs(seed[index] / ((double)M));
}

static double pf_randn(int *seed, int index) {
    double u = pf_randu(seed, index);
    double v = pf_randu(seed, index);
    double cosine = cos(2.0 * PI * v);
    double rt = -2.0 * log(u);
    return sqrt(rt) * cosine;
}

void pf_video_sequence(int *I, int IszX, int IszY, int Nfr, int *seed) {
    int max_size = IszX * IszY * Nfr;
    memset(I, 0, sizeof(int) * max_size);
    int x0 = (int)ref_roundDouble(IszY / 2.0);
    int y0 = (int)ref_roundDouble(IszX / 2.0);
    I[x0 * IszY * Nfr + y0 * Nfr + 0] = 1;

    for (int k = 1; k < Nfr; k++) {
        int xk = abs(x0 + (k - 1));
        int yk = abs(y0 - 2 * (k - 1));
        int pos = yk * IszY * Nfr + xk * Nfr + k;
        if (pos >= max_size) pos = 0;
        I[pos] = 1;
    }

    int *newMatrix = (int *)malloc(sizeof(int) * max_size);
    pf_imdilate_disk(I, IszX, IszY, Nfr, 5, newMatrix);
    memcpy(I, newMatrix, sizeof(int) * max_size);
    free(newMatrix);

    for (int i = 0; i < max_size; i++) {
        if (I[i] == 0) I[i] = 100;
        else if (I[i] == 1) I[i] = 228;
    }

    for (int x = 0; x < IszX; x++) {
        for (int y = 0; y < IszY; y++) {
            for (int z = 0; z < Nfr; z++) {
                I[x * IszY * Nfr + y * Nfr + z] += (int)(5.0 * pf_randn(seed, 0));
            }
        }
    }
}

void ref_particle_filter(
    int Nparticles, int countOnes, int max_size,
    int IszX, int IszY, int Nfr,
    const int I[],
    const int objxy[][2],
    int seed[],
    double arrayX[],
    double arrayY[],
    double weights[],
    double likelihood[],
    double CDF[],
    double u[],
    double xj[],
    double yj[],
    double out_xe[],
    double out_ye[]
) {
    long M = 2147483647;
    int A = 1103515245;
    int C = 12345;

    for (int k = 1; k < Nfr; k++) {
        // 1. Motion model
        for (int i = 0; i < Nparticles; i++) {
            int num1 = A * seed[i] + C;
            seed[i] = num1 % M;
            double res1 = (double)seed[i] / (double)M;
            double u1 = (res1 < 0.0) ? -res1 : res1;

            int num2 = A * seed[i] + C;
            seed[i] = num2 % M;
            double res2 = (double)seed[i] / (double)M;
            double v1 = (res2 < 0.0) ? -res2 : res2;

            double cosine1 = cos(2.0 * PI * v1);
            double rt1 = -2.0 * log(u1);
            double randn1 = sqrt(rt1) * cosine1;

            int num3 = A * seed[i] + C;
            seed[i] = num3 % M;
            double res3 = (double)seed[i] / (double)M;
            double u2 = (res3 < 0.0) ? -res3 : res3;

            int num4 = A * seed[i] + C;
            seed[i] = num4 % M;
            double res4 = (double)seed[i] / (double)M;
            double v2 = (res4 < 0.0) ? -res4 : res4;

            double cosine2 = cos(2.0 * PI * v2);
            double rt2 = -2.0 * log(u2);
            double randn2 = sqrt(rt2) * cosine2;

            arrayX[i] += 1.0 + 5.0 * randn1;
            arrayY[i] += -2.0 + 2.0 * randn2;
        }

        // 2. Observation Likelihood
        for (int x = 0; x < Nparticles; x++) {
            double sum_l = 0.0;
            int rx = (int)ref_roundDouble(arrayX[x]);
            int ry = (int)ref_roundDouble(arrayY[x]);

            for (int y = 0; y < countOnes; y++) {
                int indX = rx + objxy[y][1];
                int indY = ry + objxy[y][0];
                int idx = indX * IszY * Nfr + indY * Nfr + k;
                if (idx < 0) idx = -idx;
                if (idx >= max_size) idx = 0;

                double diff1 = (double)(I[idx] - 100);
                double diff2 = (double)(I[idx] - 228);
                sum_l += (diff1 * diff1 - diff2 * diff2) / 50.0;
            }
            double l = sum_l / (double)countOnes;
            likelihood[x] = l;
            weights[x] = weights[x] * exp(l);
        }

        // 3. Weight normalization
        double sumWeights = 0.0;
        for (int x = 0; x < Nparticles; x++) {
            sumWeights += weights[x];
        }
        for (int x = 0; x < Nparticles; x++) {
            weights[x] = weights[x] / sumWeights;
        }

        // 4. State estimation (Centroid)
        double xe = 0.0;
        double ye = 0.0;
        for (int x = 0; x < Nparticles; x++) {
            xe += arrayX[x] * weights[x];
            ye += arrayY[x] * weights[x];
        }
        out_xe[k] = xe;
        out_ye[k] = ye;

        // 5. Systematic Resampling (CDF Inversion)
        CDF[0] = weights[0];
        for (int x = 1; x < Nparticles; x++) {
            CDF[x] = CDF[x - 1] + weights[x];
        }

        int num_u = A * seed[0] + C;
        seed[0] = num_u % M;
        double res_u = (double)seed[0] / (double)M;
        double randu_val = (res_u < 0.0) ? -res_u : res_u;
        double u1 = (1.0 / (double)Nparticles) * randu_val;

        for (int x = 0; x < Nparticles; x++) {
            u[x] = u1 + (double)x / (double)Nparticles;
        }

        for (int j = 0; j < Nparticles; j++) {
            double target = u[j];
            int index = -1;
            for (int x = 0; x < Nparticles; x++) {
                if (CDF[x] >= target) {
                    index = x;
                    break;
                }
            }
            if (index == -1) index = Nparticles - 1;
            xj[j] = arrayX[index];
            yj[j] = arrayY[index];
        }

        for (int x = 0; x < Nparticles; x++) {
            arrayX[x] = xj[x];
            arrayY[x] = yj[x];
            weights[x] = 1.0 / (double)Nparticles;
        }
    }
}

#ifdef __cplusplus
}
#endif
