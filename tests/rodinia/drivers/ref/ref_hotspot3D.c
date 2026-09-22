#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_hotspot3D(int nx, int ny, int nz, int numiter,
                   const float *pIn,
                   float *tIn,
                   float *tOut,
                   float ce, float cw, float cn, float cs,
                   float ct, float cb, float cc,
                   float stepDivCap, float amb_temp) {
    for (int iter = 0; iter < numiter; iter += 2) {
        /* Step A: read tIn, write tOut */
        for (int z = 0; z < nz; z++) {
            for (int y = 0; y < ny; y++) {
                for (int x = 0; x < nx; x++) {
                    int c = z * ny * nx + y * nx + x;
                    float tc = tIn[c];
                    float w = (x > 0) ? tIn[c - 1] : tc;
                    float e = (x < nx - 1) ? tIn[c + 1] : tc;
                    float n = (y > 0) ? tIn[c - nx] : tc;
                    float s = (y < ny - 1) ? tIn[c + nx] : tc;
                    float b = (z > 0) ? tIn[c - nx * ny] : tc;
                    float t = (z < nz - 1) ? tIn[c + nx * ny] : tc;

                    tOut[c] = tc * cc + n * cn + s * cs +
                              e * ce + w * cw + t * ct +
                              b * cb + stepDivCap * pIn[c] + ct * amb_temp;
                }
            }
        }
        /* Step B: read tOut, write tIn */
        if (iter + 1 < numiter) {
            for (int z = 0; z < nz; z++) {
                for (int y = 0; y < ny; y++) {
                    for (int x = 0; x < nx; x++) {
                        int c = z * ny * nx + y * nx + x;
                        float tc = tOut[c];
                        float w = (x > 0) ? tOut[c - 1] : tc;
                        float e = (x < nx - 1) ? tOut[c + 1] : tc;
                        float n = (y > 0) ? tOut[c - nx] : tc;
                        float s = (y < ny - 1) ? tOut[c + nx] : tc;
                        float b = (z > 0) ? tOut[c - nx * ny] : tc;
                        float t = (z < nz - 1) ? tOut[c + nx * ny] : tc;

                        tIn[c] = tc * cc + n * cn + s * cs +
                                 e * ce + w * cw + t * ct +
                                 b * cb + stepDivCap * pIn[c] + ct * amb_temp;
                    }
                }
            }
        }
    }
}

#ifdef __cplusplus
}
#endif
