#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b) ((a) <= (b) ? (a) : (b))

void ref_pathfinder(int rows, int cols,
                    const int *wall,
                    int *src,
                    int *dst) {
    for (int t = 0; t < rows - 1; t++) {
        int *temp = src;
        src = dst;
        dst = temp;
        for (int n = 0; n < cols; n++) {
            int min_val = src[n];
            if (n > 0 && src[n - 1] < min_val) min_val = src[n - 1];
            if (n < cols - 1 && src[n + 1] < min_val) min_val = src[n + 1];
            dst[n] = wall[(t + 1) * cols + n] + min_val;
        }
    }
    if ((rows - 1) % 2 == 1) {
        for (int n = 0; n < cols; n++) {
            int t = dst[n];
            dst[n] = src[n];
            src[n] = t;
        }
    }
}

#ifdef __cplusplus
}
#endif
