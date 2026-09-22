#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int max3(int a, int b, int c) {
  int m = (a > b) ? a : b;
  return (m > c) ? m : c;
}

void ref_nw(int max_rows, int max_cols,
            int *input_itemsets,
            const int *reference,
            int penalty) {
  for (int i = 1; i < max_rows; i++) {
    for (int j = 1; j < max_cols; j++) {
      int diag = input_itemsets[(i - 1) * max_cols + (j - 1)] + reference[i * max_cols + j];
      int left = input_itemsets[i * max_cols + (j - 1)] - penalty;
      int up   = input_itemsets[(i - 1) * max_cols + j] - penalty;
      input_itemsets[i * max_cols + j] = max3(diag, left, up);
    }
  }
}

#ifdef __cplusplus
}
#endif
