#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_lud(int size, float *a) {
  for (int i = 0; i < size; i++) {
    for (int j = i; j < size; j++) {
      float sum = a[i * size + j];
      for (int k = 0; k < i; k++) {
        sum -= a[i * size + k] * a[k * size + j];
      }
      a[i * size + j] = sum;
    }

    for (int j = i + 1; j < size; j++) {
      float sum = a[j * size + i];
      for (int k = 0; k < i; k++) {
        sum -= a[j * size + k] * a[k * size + i];
      }
      a[j * size + i] = sum / a[i * size + i];
    }
  }
}

#ifdef __cplusplus
}
#endif
