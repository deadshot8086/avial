#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OPEN 10000.0f

void ref_nn(int n, int k,
            const float *lat,
            const float *lng,
            float target_lat,
            float target_long,
            int *nearest_idx,
            float *nearest_dist) {
  for (int j = 0; j < k; j++) {
    nearest_dist[j] = OPEN;
    nearest_idx[j] = -1;
  }

  for (int i = 0; i < n; i++) {
    float dlat = lat[i] - target_lat;
    float dlng = lng[i] - target_long;
    float d = sqrtf(dlat * dlat + dlng * dlng);

    float max_dist = -1.0f;
    int max_pos = 0;
    for (int j = 0; j < k; j++) {
      if (nearest_dist[j] > max_dist) {
        max_dist = nearest_dist[j];
        max_pos = j;
      }
    }

    if (d < nearest_dist[max_pos]) {
      nearest_dist[max_pos] = d;
      nearest_idx[max_pos] = i;
    }
  }
}

#ifdef __cplusplus
}
#endif
