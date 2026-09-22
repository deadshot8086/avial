// Flat-pointer reference for the Parboil mri-gridding kernel.

#include <math.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REF_MG_MAX(x, y) ((x < y) ? y : x)
#define REF_MG_MIN(x, y) ((x > y) ? y : x)

/* polynomials taken from the original CPU_kernels.c */
static float ref_kernel_value_CPU(float v) {
  float rValue = 0;

  const float z = v * v;

  float num = (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z *
  (z * 0.210580722890567e-22f  + 0.380715242345326e-19f ) +
   0.479440257548300e-16f) + 0.435125971262668e-13f ) +
   0.300931127112960e-10f) + 0.160224679395361e-7f  ) +
   0.654858370096785e-5f)  + 0.202591084143397e-2f  ) +
   0.463076284721000e0f)   + 0.754337328948189e2f   ) +
   0.830792541809429e4f)   + 0.571661130563785e6f   ) +
   0.216415572361227e8f)   + 0.356644482244025e9f   ) +
   0.144048298227235e10f);

  float den = (z * (z * (z - 0.307646912682801e4f) + 0.347626332405882e7f) -
               0.144048298227235e10f);

  rValue = -num / den;

  return rValue;
}

static float ref_kernel_value_LUT(float v, const float *LUT, int sizeLUT,
                                  float _1overCutoff2) {
  unsigned int k0;
  float v0;

  v *= (float)sizeLUT;
  k0 = (unsigned int)(v * _1overCutoff2);
  v0 = ((float)k0) / _1overCutoff2;
  return LUT[k0] + ((v - v0) * (LUT[k0 + 1] - LUT[k0]) / _1overCutoff2);
}

void ref_mri_gridding(int n,
                      int size_x_in, int size_y_in, int size_z_in,
                      float cutoff, float cutoff2, float _1overCutoff2,
                      float beta,
                      int sizeLUT, int useLUT,
                      const float *sample,
                      const float *LUT,
                      float *gridData,
                      float *sampleDensity) {
  unsigned int NxL, NxH;
  unsigned int NyL, NyH;
  unsigned int NzL, NzH;

  int nx, ny, nz;

  float w;
  unsigned int idx, idx0;
  unsigned int idxZ, idxY;

  float Dx2[100];
  float Dy2[100];
  float Dz2[100];
  float *dx2 = NULL;
  float *dy2 = NULL;
  float *dz2 = NULL;

  float dy2dz2;
  float v;

  unsigned int size_x = (unsigned int)size_x_in;
  unsigned int size_y = (unsigned int)size_y_in;
  unsigned int size_z = (unsigned int)size_z_in;

  int i;
  for (i = 0; i < n; i++) {
    /* ReconstructionSample pt = sample[i]; field order real,imag,kX,kY,kZ,sdc */
    float pt_real = sample[6 * i + 0];
    float pt_imag = sample[6 * i + 1];
    float kx = sample[6 * i + 2];
    float ky = sample[6 * i + 3];
    float kz = sample[6 * i + 4];
    float pt_sdc = sample[6 * i + 5];

    NxL = REF_MG_MAX((kx - cutoff), 0.0);
    NxH = REF_MG_MIN((kx + cutoff), size_x - 1.0);

    NyL = REF_MG_MAX((ky - cutoff), 0.0);
    NyH = REF_MG_MIN((ky + cutoff), size_y - 1.0);

    NzL = REF_MG_MAX((kz - cutoff), 0.0);
    NzH = REF_MG_MIN((kz + cutoff), size_z - 1.0);

    if ((pt_real != 0.0 || pt_imag != 0.0) && pt_sdc != 0.0) {
      for (dz2 = Dz2, nz = NzL; nz <= (int)NzH; ++nz, ++dz2) {
        *dz2 = ((kz - nz) * (kz - nz));
      }
      for (dx2 = Dx2, nx = NxL; nx <= (int)NxH; ++nx, ++dx2) {
        *dx2 = ((kx - nx) * (kx - nx));
      }
      for (dy2 = Dy2, ny = NyL; ny <= (int)NyH; ++ny, ++dy2) {
        *dy2 = ((ky - ny) * (ky - ny));
      }

      idxZ = (NzL - 1) * size_x * size_y;
      for (dz2 = Dz2, nz = NzL; nz <= (int)NzH; ++nz, ++dz2) {
        /* linear offset into 3-D matrix to get to z position */
        idxZ += size_x * size_y;

        idxY = (NyL - 1) * size_x;

        if ((*dz2) < cutoff2) {
          for (dy2 = Dy2, ny = NyL; ny <= (int)NyH; ++ny, ++dy2) {
            /* linear offset IN ADDITION to idxZ to get to Y position */
            idxY += size_x;

            dy2dz2 = (*dz2) + (*dy2);

            idx0 = idxY + idxZ;

            if (dy2dz2 < cutoff2) {
              for (dx2 = Dx2, nx = NxL; nx <= (int)NxH; ++nx, ++dx2) {
                /* value to evaluate kernel at */
                v = dy2dz2 + (*dx2);

                if (v < cutoff2) {
                  /* linear index of (x,y,z) point */
                  idx = nx + idx0;

                  /* kernel weighting value */
                  if (useLUT) {
                    w = ref_kernel_value_LUT(v, LUT, sizeLUT, _1overCutoff2) *
                        pt_sdc;
                  } else {
                    w = ref_kernel_value_CPU(
                            beta * sqrt(1.0 - (v * _1overCutoff2))) *
                        pt_sdc;
                  }

                  /* grid data: interleaved cmplx {real, imag} */
                  gridData[2 * idx + 0] += (w * pt_real);
                  gridData[2 * idx + 1] += (w * pt_imag);

                  /* estimate sample density */
                  sampleDensity[idx] += 1.0;
                }
              }
            }
          }
        }
      }
    }
  }
}

#ifdef __cplusplus
}
#endif
