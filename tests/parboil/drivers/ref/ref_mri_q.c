// Reference for mri_q, independently derived

#include <math.h>

#define REF_PIx2 6.2831853071795864769252867665590058f

#ifdef __cplusplus
extern "C" {
#endif

void ref_mri_q(int numK, int numX,
               float *kVals,
               const float *phiR, const float *phiI,
               const float *x, const float *y, const float *z,
               float *Qr, float *Qi) {
  int indexK, indexX;

  /* ComputePhiMagCPU */
  for (indexK = 0; indexK < numK; indexK++) {
    float real = phiR[indexK];
    float imag = phiI[indexK];
    kVals[indexK * 4 + 3] = real * real + imag * imag;
  }

  /* ComputeQCPU */
  for (indexK = 0; indexK < numK; indexK++) {
    for (indexX = 0; indexX < numX; indexX++) {
      float expArg = REF_PIx2 * (kVals[indexK * 4 + 0] * x[indexX] +
                                 kVals[indexK * 4 + 1] * y[indexX] +
                                 kVals[indexK * 4 + 2] * z[indexX]);

      float cosArg = cosf(expArg);
      float sinArg = sinf(expArg);

      float phi = kVals[indexK * 4 + 3];
      Qr[indexX] += phi * cosArg;
      Qi[indexX] += phi * sinArg;
    }
  }
}

#ifdef __cplusplus
}
#endif
