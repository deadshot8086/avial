#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_gaussian(int Size, float *m, float *a, float *b, float *x) {
  // Phase 1: Forward Elimination
  for (int t = 0; t < Size - 1; ++t) {
    for (int i = t + 1; i < Size; ++i) {
      m[i * Size + t] = a[i * Size + t] / a[t * Size + t];
    }

    for (int i = t + 1; i < Size; ++i) {
      for (int j = t; j < Size; ++j) {
        a[i * Size + j] -= m[i * Size + t] * a[t * Size + j];
      }
      b[i] -= m[i * Size + t] * b[t];
    }
  }

  // Phase 2: Back Substitution
  for (int i = 0; i < Size; ++i) {
    float sum = b[Size - 1 - i];
    for (int j = 0; j < i; ++j) {
      sum -= a[(Size - 1 - i) * Size + (Size - 1 - j)] * x[Size - 1 - j];
    }
    x[Size - 1 - i] = sum / a[(Size - 1 - i) * Size + (Size - 1 - i)];
  }
}

#ifdef __cplusplus
}
#endif
