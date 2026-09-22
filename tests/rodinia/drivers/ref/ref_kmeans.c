#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_kmeans(int npoints, int nfeatures, int nclusters, float threshold,
                const float feature[npoints][nfeatures],
                float clusters[nclusters][nfeatures],
                int membership[npoints],
                float new_centers[nclusters][nfeatures],
                int new_centers_len[nclusters]) {
    float delta;
    do {
        delta = 0.0f;
        for (int i = 0; i < nclusters; i++) {
            new_centers_len[i] = 0;
            for (int f = 0; f < nfeatures; f++) {
                new_centers[i][f] = 0.0f;
            }
        }

        for (int i = 0; i < npoints; i++) {
            int index = 0;
            float min_dist = 1.0e30f;

            for (int c = 0; c < nclusters; c++) {
                float dist = 0.0f;
                for (int f = 0; f < nfeatures; f++) {
                    float diff = feature[i][f] - clusters[c][f];
                    dist += diff * diff;
                }
                if (dist < min_dist) {
                    min_dist = dist;
                    index = c;
                }
            }

            if (membership[i] != index) {
                delta += 1.0f;
            }
            membership[i] = index;
            new_centers_len[index] += 1;
            for (int f = 0; f < nfeatures; f++) {
                new_centers[index][f] += feature[i][f];
            }
        }

        for (int c = 0; c < nclusters; c++) {
            if (new_centers_len[c] > 0) {
                for (int f = 0; f < nfeatures; f++) {
                    clusters[c][f] = new_centers[c][f] / (float)new_centers_len[c];
                }
            }
        }
    } while (delta > threshold);
}

#ifdef __cplusplus
}
#endif
