#define DHIR_BENCH_NAME "nn"
#include "dhir_bench.h"
#include <string>
#include <vector>

extern "C" {
void nn(int32_t, int32_t,
        float *, float *, int64_t, int64_t, int64_t,
        float *, float *, int64_t, int64_t, int64_t,
        float, float,
        int32_t *, int32_t *, int64_t, int64_t, int64_t,
        float *, float *, int64_t, int64_t, int64_t);

void ref_nn(int n, int k,
            const float *lat,
            const float *lng,
            float target_lat,
            float target_long,
            int *nearest_idx,
            float *nearest_dist);
}

static const int REC_LENGTH = 49;
static const int LATITUDE_POS = 28;

static int num_records = 0;
static int k_neighbors = 5;
static float target_lat = 30.0f;
static float target_long = 90.0f;

static float *lat_arr = nullptr;
static float *lng_arr = nullptr;
static int32_t *nearest_idx = nullptr;
static int32_t *nearest_idx_ref = nullptr;
static float *nearest_dist = nullptr;
static float *nearest_dist_ref = nullptr;

static std::vector<std::string> records_data;

static FILE *open_resolved(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp) return fp;

    // Try relative to workspace rodinia dirs
    std::string p1 = std::string("vendor/rodinia/rodinia_3.1/openmp/nn/") + path;
    fp = fopen(p1.c_str(), "r");
    if (fp) return fp;

    std::string p2 = std::string("vendor/rodinia/rodinia_3.1/data/nn/") + path;
    fp = fopen(p2.c_str(), "r");
    if (fp) return fp;

    // Strip ../../data/nn/
    std::string sp(path);
    size_t idx = sp.find("cane");
    if (idx != std::string::npos) {
        std::string p3 = std::string("vendor/rodinia/rodinia_3.1/data/nn/") + sp.substr(idx);
        fp = fopen(p3.c_str(), "r");
        if (fp) return fp;
    }

    return nullptr;
}

static void read_single_db(const char *filename, int max_cap,
                           std::vector<float> &lats, std::vector<float> &lngs) {
    FILE *fp = open_resolved(filename);
    if (!fp) {
        fprintf(stderr, "[nn] Warning: Cannot open database file %s\n", filename);
        return;
    }
    char record[128];
    while (fgets(record, sizeof(record), fp)) {
        if (max_cap > 0 && (int)lats.size() >= max_cap) {
            break;
        }
        if ((int)strlen(record) >= LATITUDE_POS) {
            float tmp_lat = 0.0f, tmp_long = 0.0f;
            if (sscanf(record + LATITUDE_POS, "%f %f", &tmp_lat, &tmp_long) == 2) {
                // Strip trailing newline from record line
                size_t len = strlen(record);
                while (len > 0 && (record[len - 1] == '\n' || record[len - 1] == '\r')) {
                    record[--len] = '\0';
                }
                records_data.push_back(std::string(record));
                lats.push_back(tmp_lat);
                lngs.push_back(tmp_long);
            }
        }
    }
    fclose(fp);
}

static void load_dataset(void) {
    int max_cap = -1;
    const char *cap_env = getenv("NN_MAX_RECORDS");
    if (cap_env) {
        max_cap = atoi(cap_env);
    }

    const char *k_env = getenv("NN_K");
    if (k_env) {
        k_neighbors = atoi(k_env);
        if (k_neighbors < 1) k_neighbors = 5;
    }

    const char *lat_env = getenv("NN_TARGET_LAT");
    if (lat_env) target_lat = atof(lat_env);

    const char *lng_env = getenv("NN_TARGET_LONG");
    if (lng_env) target_long = atof(lng_env);

    const char *flist_path = getenv("NN_FILELIST");
    if (!flist_path) flist_path = getenv("NN_DATASET");

    std::vector<float> lats, lngs;
    records_data.clear();

    // Default search paths for filelist as per Rodinia 3.1
    if (!flist_path) {
        FILE *fcheck = open_resolved("vendor/rodinia/rodinia_3.1/data/nn/filelist.txt");
        if (fcheck) {
            fclose(fcheck);
            flist_path = "vendor/rodinia/rodinia_3.1/data/nn/filelist.txt";
        } else {
            flist_path = "vendor/rodinia/rodinia_3.1/openmp/nn/filelist_4";
        }
    }

    // Check if flist_path is a direct .db file or a filelist
    std::string s_path(flist_path);
    if (s_path.size() >= 3 && s_path.substr(s_path.size() - 3) == ".db") {
        read_single_db(flist_path, max_cap, lats, lngs);
    } else {
        FILE *flist = open_resolved(flist_path);
        if (flist) {
            std::string base_dir = "";
            size_t last_slash = s_path.find_last_of("/\\");
            if (last_slash != std::string::npos) {
                base_dir = s_path.substr(0, last_slash + 1);
            }

            char dbname[256];
            while (fscanf(flist, "%255s\n", dbname) == 1) {
                std::string full_db = base_dir + dbname;
                FILE *db_test = open_resolved(full_db.c_str());
                if (db_test) {
                    fclose(db_test);
                    read_single_db(full_db.c_str(), max_cap, lats, lngs);
                } else {
                    read_single_db(dbname, max_cap, lats, lngs);
                }
                if (max_cap > 0 && (int)lats.size() >= max_cap) {
                    break;
                }
            }
            fclose(flist);
        } else {
            read_single_db("vendor/rodinia/rodinia_3.1/data/nn/cane4_0.db", max_cap, lats, lngs);
        }
    }

    num_records = (int)lats.size();
    if (num_records == 0) {
        fprintf(stderr, "[nn] Fatal: No records read from dataset!\n");
        exit(1);
    }

    lat_arr = dhir_alloc(num_records);
    lng_arr = dhir_alloc(num_records);
    for (int i = 0; i < num_records; i++) {
        lat_arr[i] = lats[i];
        lng_arr[i] = lngs[i];
    }
}

static void bench_alloc(void) {
    load_dataset();

    nearest_idx = dhir_alloc_int(k_neighbors);
    nearest_idx_ref = dhir_alloc_int(k_neighbors);
    nearest_dist = dhir_alloc(k_neighbors);
    nearest_dist_ref = dhir_alloc(k_neighbors);

    for (int j = 0; j < k_neighbors; j++) {
        nearest_idx[j] = -1;
        nearest_idx_ref[j] = -1;
        nearest_dist[j] = 10000.0f;
        nearest_dist_ref[j] = 10000.0f;
    }
}

static void bench_call(void) {
    nn(num_records, k_neighbors,
       MR1(lat_arr, num_records),
       MR1(lng_arr, num_records),
       target_lat, target_long,
       MR1(nearest_idx, k_neighbors),
       MR1(nearest_dist, k_neighbors));
}

static void bench_reference(void) {
    ref_nn(num_records, k_neighbors,
           lat_arr, lng_arr,
           target_lat, target_long,
           nearest_idx_ref, nearest_dist_ref);
}

static int bench_check(void) {
    int errors = 0;
    errors += dhir_compare_int("nearest_idx", nearest_idx, nearest_idx_ref, k_neighbors);
    errors += dhir_compare("nearest_dist", nearest_dist, nearest_dist_ref, k_neighbors, DHIR_EPS);

    return errors;
}

static void bench_free(void) {
    free(lat_arr);
    free(lng_arr);
    free(nearest_idx);
    free(nearest_idx_ref);
    free(nearest_dist);
    free(nearest_dist_ref);
    records_data.clear();
}
