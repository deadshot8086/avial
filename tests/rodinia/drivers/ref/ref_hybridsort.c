#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_hybridsort(const float *input,
                    int size,
                    const float *pivots,
                    int divisions,
                    int *indices,
                    int *bucket_counts) {
    for (int i = 0; i < size; ++i) {
        float elem = input[i];
        int idx = divisions / 2 - 1;
        int jump = divisions / 4;
        float piv = pivots[idx];

        while (jump >= 1) {
            idx = (elem < piv) ? (idx - jump) : (idx + jump);
            piv = pivots[idx];
            jump /= 2;
        }
        idx = (elem < piv) ? idx : (idx + 1);

        indices[i] = idx;
        bucket_counts[idx]++;
    }
}

#ifdef __cplusplus
}
#endif
