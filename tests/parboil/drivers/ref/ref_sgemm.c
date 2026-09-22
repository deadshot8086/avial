// Independent flat-pointer reference for Parboil sgemm.

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_sgemm(int m, int n, int k, float alpha, float beta,
               const float *A, const float *B, float *C) {
  const int lda = m;
  const int ldb = n;
  const int ldc = m;

  for (int mm = 0; mm < m; ++mm) {
    for (int nn = 0; nn < n; ++nn) {
      float c = 0.0f;
      for (int i = 0; i < k; ++i) {
        float a = A[mm + i * lda];
        float b = B[nn + i * ldb];
        c += a * b;
      }
      C[mm + nn * ldc] = C[mm + nn * ldc] * beta + alpha * c;
    }
  }
}

#ifdef __cplusplus
}
#endif
