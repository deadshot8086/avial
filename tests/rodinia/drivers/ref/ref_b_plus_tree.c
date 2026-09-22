#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_b_plus_tree(int count, int maxheight, int order,
                     long knodes_elem,
                     const int *knodes_keys,
                     const int *knodes_indices,
                     const int *records_val,
                     const int *keys,
                     long *currKnode,
                     long *offset,
                     int *ans_val) {
  int order_width = order + 1;
  for (int bid = 0; bid < count; bid++) {
    int target_key = keys[bid];

    for (int i = 0; i < maxheight; i++) {
      int curr = (int)currKnode[bid];
      int off = (int)offset[bid];

      for (int j = 0; j < order; j++) {
        int k1 = knodes_keys[curr * order_width + j];
        int k2 = knodes_keys[curr * order_width + (j + 1)];

        if (k1 <= target_key && k2 > target_key) {
          int next_idx = knodes_indices[off * order_width + j];
          if ((long)next_idx < knodes_elem) {
            offset[bid] = next_idx;
          }
        }
      }
      currKnode[bid] = offset[bid];
    }

    int leaf = (int)currKnode[bid];
    for (int j = 0; j < order; j++) {
      if (knodes_keys[leaf * order_width + j] == target_key) {
        int rec_idx = knodes_indices[leaf * order_width + j];
        ans_val[bid] = records_val[rec_idx];
      }
    }
  }
}

#ifdef __cplusplus
}
#endif
