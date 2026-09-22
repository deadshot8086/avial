#define DHIR_BENCH_NAME "kmeans"
#include "dhir_bench.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

extern "C" {
void kmeans(int32_t, int32_t, int32_t, float,
            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
            int32_t *, int32_t *, int64_t, int64_t, int64_t,
            float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
            int32_t *, int32_t *, int64_t, int64_t, int64_t);

void ref_kmeans(int npoints, int nfeatures, int nclusters, float threshold,
                const float *feature,
                float *clusters,
                int *membership,
                float *new_centers,
                int *new_centers_len);
}

static int npoints = 0;
static int nfeatures = 0;
static int nclusters = 5;
static float threshold = 0.001f;

static float *feature;
static float *clusters, *clusters_ref;
static int *membership, *membership_ref;
static float *new_centers, *new_centers_ref;
static int *new_centers_len, *new_centers_len_ref;

static void read_data(const char *infile) {
    FILE *fp = fopen(infile, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", infile);
        exit(1);
    }
    char line[1024];
    npoints = 0;
    nfeatures = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strtok(line, " \t\n") != NULL)
            npoints++;
    }
    rewind(fp);
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strtok(line, " \t\n") != NULL) {
            /* Ignore point ID (first token); count remaining feature tokens */
            while (strtok(NULL, " ,\t\n") != NULL)
                nfeatures++;
            break;
        }
    }

    feature = dhir_alloc((long)npoints * nfeatures);
    rewind(fp);
    int pt = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strtok(line, " \t\n") == NULL) continue;
        for (int f = 0; f < nfeatures; f++) {
            char *tok = strtok(NULL, " ,\t\n");
            feature[pt * nfeatures + f] = tok ? (float)atof(tok) : 0.0f;
        }
        pt++;
    }
    fclose(fp);
}

static void bench_alloc(void) {
    const char *infile = getenv("KMEANS_INPUT");
    if (!infile) infile = getenv("KMEANS_FILE");
    if (!infile) {
        infile = "vendor/rodinia/rodinia_3.1/data/kmeans/kdd_cup";
        FILE *test_fp = fopen(infile, "r");
        if (!test_fp) {
            infile = "vendor/rodinia/rodinia_3.1/data/kmeans/100";
        } else {
            fclose(test_fp);
        }
    }

    const char *env_k = getenv("KMEANS_K");
    if (!env_k) env_k = getenv("KMEANS_CLUSTERS");
    if (env_k && atoi(env_k) > 0) nclusters = atoi(env_k);

    const char *env_t = getenv("KMEANS_THRESHOLD");
    if (env_t && atof(env_t) > 0.0) threshold = (float)atof(env_t);

    read_data(infile);

    clusters = dhir_alloc((long)nclusters * nfeatures);
    clusters_ref = dhir_alloc((long)nclusters * nfeatures);

    membership = dhir_alloc_int(npoints);
    membership_ref = dhir_alloc_int(npoints);

    new_centers = dhir_alloc((long)nclusters * nfeatures);
    new_centers_ref = dhir_alloc((long)nclusters * nfeatures);

    new_centers_len = dhir_alloc_int(nclusters);
    new_centers_len_ref = dhir_alloc_int(nclusters);

    // Initialize cluster centers to first nclusters data points (matching Rodinia kmeans_serial)
    for (int c = 0; c < nclusters; c++) {
        for (int f = 0; f < nfeatures; f++) {
            clusters[c * nfeatures + f] = feature[c * nfeatures + f];
            clusters_ref[c * nfeatures + f] = feature[c * nfeatures + f];
        }
        new_centers_len[c] = 0;
        new_centers_len_ref[c] = 0;
    }

    for (int i = 0; i < npoints; i++) {
        membership[i] = -1;
        membership_ref[i] = -1;
    }
}

static void bench_call(void) {
    kmeans(npoints, nfeatures, nclusters, threshold,
           MR2(feature, npoints, nfeatures),
           MR2(clusters, nclusters, nfeatures),
           MR1(membership, npoints),
           MR2(new_centers, nclusters, nfeatures),
           MR1(new_centers_len, nclusters));
}

static void bench_reference(void) {
    ref_kmeans(npoints, nfeatures, nclusters, threshold,
               feature, clusters_ref,
               membership_ref,
               new_centers_ref, new_centers_len_ref);
}

static int bench_check(void) {
    int errors = 0;
    errors += dhir_compare_int("membership", membership, membership_ref, npoints);
    errors += dhir_compare("clusters", clusters, clusters_ref, (long)nclusters * nfeatures, DHIR_EPS);
    return errors;
}

static void bench_free(void) {
    if (feature) free(feature);
    if (clusters) free(clusters);
    if (clusters_ref) free(clusters_ref);
    if (membership) free(membership);
    if (membership_ref) free(membership_ref);
    if (new_centers) free(new_centers);
    if (new_centers_ref) free(new_centers_ref);
    if (new_centers_len) free(new_centers_len);
    if (new_centers_len_ref) free(new_centers_len_ref);
}
