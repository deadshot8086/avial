#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_srad(int Nr, int Nc, int niter, float lambda,
              int r1, int r2, int c1, int c2,
              float *image,
              float *c,
              float *dN, float *dS, float *dW, float *dE) {
    float NeROI = (float)((r2 - r1 + 1) * (c2 - c1 + 1));

    for (int iter = 0; iter < niter; iter++) {
        /* 1. ROI statistics reduction matching srad_v1 loop order */
        float sum = 0.0f;
        float sum2 = 0.0f;
        for (int i = r1; i <= r2; i++) {
            for (int j = c1; j <= c2; j++) {
                float tmp = image[j * Nr + i];
                sum += tmp;
                sum2 += tmp * tmp;
            }
        }
        float meanROI = sum / NeROI;
        float varROI = (sum2 / NeROI) - meanROI * meanROI;
        float q0sqr = varROI / (meanROI * meanROI);

        /* 2. Directional derivatives & diffusion coefficient */
        for (int j = 0; j < Nc; j++) {
            for (int i = 0; i < Nr; i++) {
                float Jc = image[j * Nr + i];

                float dn = ((i > 0) ? image[j * Nr + (i - 1)] : image[j * Nr + 0]) - Jc;
                float ds = ((i < Nr - 1) ? image[j * Nr + (i + 1)] : image[j * Nr + (Nr - 1)]) - Jc;
                float dw = ((j > 0) ? image[(j - 1) * Nr + i] : image[0 * Nr + i]) - Jc;
                float de = ((j < Nc - 1) ? image[(j + 1) * Nr + i] : image[(Nc - 1) * Nr + i]) - Jc;

                dN[j * Nr + i] = dn;
                dS[j * Nr + i] = ds;
                dW[j * Nr + i] = dw;
                dE[j * Nr + i] = de;

                float G2 = (dn * dn + ds * ds + dw * dw + de * de) / (Jc * Jc);
                float L = (dn + ds + dw + de) / Jc;

                float num = (0.5 * G2) - ((1.0 / 16.0) * (L * L));
                float den = 1 + (.25 * L);
                float qsqr = num / (den * den);

                den = (qsqr - q0sqr) / (q0sqr * (1 + q0sqr));
                float c_val = 1.0 / (1.0 + den);
                if (c_val < 0.0f) c_val = 0.0f;
                if (c_val > 1.0f) c_val = 1.0f;
                c[j * Nr + i] = c_val;
            }
        }

        /* 3. Divergence and image update */
        for (int j = 0; j < Nc; j++) {
            for (int i = 0; i < Nr; i++) {
                float cN = c[j * Nr + i];
                float cS = (i < Nr - 1) ? c[j * Nr + (i + 1)] : c[j * Nr + (Nr - 1)];
                float cW = c[j * Nr + i];
                float cE = (j < Nc - 1) ? c[(j + 1) * Nr + i] : c[(Nc - 1) * Nr + i];

                float D = cN * dN[j * Nr + i] + cS * dS[j * Nr + i] + cW * dW[j * Nr + i] + cE * dE[j * Nr + i];
                image[j * Nr + i] = image[j * Nr + i] + 0.25 * lambda * D;
            }
        }
    }

    /* 4. Log compression and scale to 0-255 matching Rodinia srad_v1 */
    for (int j = 0; j < Nc; j++) {
        for (int i = 0; i < Nr; i++) {
            image[j * Nr + i] = log(image[j * Nr + i]) * 255;
        }
    }
}

#ifdef __cplusplus
}
#endif
