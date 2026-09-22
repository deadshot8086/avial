#define DHIR_BENCH_NAME "mummergpu"
#include "dhir_bench.h"
#include "ref/mummergpu_tree.h"
#include <fstream>
#include <vector>
#include <string>

extern "C" void mummergpu_match(
    int32_t num_queries,
    const int *q_off_alloc, const int *q_off_align, int64_t q_off_offset, int64_t q_off_size, int64_t q_off_stride,
    const int *q_len_alloc, const int *q_len_align, int64_t q_len_offset, int64_t q_len_size, int64_t q_len_stride,
    const char *q_alloc, const char *q_align, int64_t q_offset, int64_t q_size, int64_t q_stride,
    const char *ref_alloc, const char *ref_align, int64_t ref_offset, int64_t ref_size, int64_t ref_stride,
    int32_t min_match_len,
    const int *ns_alloc, const int *ns_align, int64_t ns_offset, int64_t ns_size, int64_t ns_stride,
    const int *ne_alloc, const int *ne_align, int64_t ne_offset, int64_t ne_size, int64_t ne_stride,
    const int *nsu_alloc, const int *nsu_align, int64_t nsu_offset, int64_t nsu_size, int64_t nsu_stride,
    const int *nc_alloc, const int *nc_align, int64_t nc_offset, int64_t nc_size0, int64_t nc_size1, int64_t nc_stride0, int64_t nc_stride1,
    int *rn_alloc, int *rn_align, int64_t rn_offset, int64_t rn_size, int64_t rn_stride,
    int *re_alloc, int *re_align, int64_t re_offset, int64_t re_size, int64_t re_stride,
    int *rq_alloc, int *rq_align, int64_t rq_offset, int64_t rq_size, int64_t rq_stride
);

extern "C" void ref_mummergpu(
    int num_queries,
    const int *query_offsets,
    const int *query_lengths,
    const char *queries,
    const char *ref,
    int min_match_len,
    const int *node_start,
    const int *node_end,
    const int *node_suffix,
    const int node_children[][4],
    int *result_node,
    int *result_edge_len,
    int *result_qry_len
);

static int32_t num_queries = 0;
static int32_t min_match_len = 20;
static int *query_offsets = nullptr;
static int *query_lengths = nullptr;
static char *queries_buf = nullptr;
static size_t total_buf_size = 0;

static char *ref_dna = nullptr;
static size_t ref_len = 0;

static int total_nodes = 0;
static int *node_start = nullptr;
static int *node_end = nullptr;
static int *node_suffix = nullptr;
static int *node_children = nullptr;

static int *mlir_res_node = nullptr;
static int *mlir_res_edge = nullptr;
static int *mlir_res_qry = nullptr;

static int *ref_res_node = nullptr;
static int *ref_res_edge = nullptr;
static int *ref_res_qry = nullptr;

static void bench_alloc(void) {
    const char *ref_candidates[] = {
        "vendor/rodinia/rodinia_3.1/data/mummergpu/NC_003997.20k.fna",
    };
    const char *qry_candidates[] = {
        "vendor/rodinia/rodinia_3.1/data/mummergpu/NC_003997_q25bp.50k.fna",
    };

    const char *ref_path = nullptr;
    for (auto c : ref_candidates) {
        FILE *f = fopen(c, "r");
        if (f) { fclose(f); ref_path = c; break; }
    }

    const char *qry_path = nullptr;
    for (auto c : qry_candidates) {
        FILE *f = fopen(c, "r");
        if (f) { fclose(f); qry_path = c; break; }
    }

    // Reference: the whole shipped sequence (NC_003997.20k.fna is 1,399,930
    // bases).  MUMMERGPU_REF_BASES truncates it, for a quick smoke run only:
    // a short reference gives a shallow tree that almost no 25bp read reaches
    // min_match_len in, so the benchmark stops exercising suffix-tree walking.
    long ref_cap = 0;
    if (const char *e = getenv("MUMMERGPU_REF_BASES")) ref_cap = atol(e);

    std::string ref_str = "s"; // 1-based indexing prefix
    if (ref_path) {
        std::ifstream rf(ref_path);
        std::string line;
        while (std::getline(rf, line)) {
            if (line.empty() || line[0] == '>') continue;
            for (char c : line) {
                if (c >= 'A' && c <= 'Z') ref_str += c;
                else if (c >= 'a' && c <= 'z') ref_str += (c - 'a' + 'A');
            }
            if (ref_cap > 0 && (long)ref_str.length() > ref_cap) break;
        }
        if (ref_cap > 0 && (long)ref_str.length() > ref_cap + 1)
            ref_str.resize(ref_cap + 1);
    } else {
        // Pseudo-random rather than a repeating ACGT pattern: a period-4 string
        // has a degenerate suffix tree (depth n/4, so ~350k levels at this
        // size) and the recursive flatten_tree/destructor overflow the stack.
        // Real DNA keeps the tree ~22 deep.
        const char bases[] = "ACGT";
        long n = ref_cap > 0 ? ref_cap : 1399930;
        uint64_t st = 0x9E3779B97F4A7C15ull;
        for (long i = 0; i < n; i++) {
            st ^= st << 13; st ^= st >> 7; st ^= st << 17;
            ref_str += bases[st & 3];
        }
    }
    ref_str += '$';
    ref_len = ref_str.length();

    ref_dna = (char *)aligned_alloc(64, (ref_len + 63) / 64 * 64);
    memcpy(ref_dna, ref_str.data(), ref_len);

    SuffixNode::s_nodecount = 0;
    SuffixTree tree(ref_str.c_str());
    tree.buildUkkonen();
    total_nodes = SuffixNode::s_nodecount;

    std::vector<int> n_start(total_nodes + 1, 0);
    std::vector<int> n_end(total_nodes + 1, 0);
    std::vector<int> n_suffix(total_nodes + 1, 0);
    std::vector<int> n_children((total_nodes + 1) * 4, 0);
    flatten_tree(tree.m_root, n_start, n_end, n_suffix, n_children);

    // aligned_alloc needs the size to be a multiple of the alignment, and at
    // the full 2.3M-node tree these arrays are no longer incidentally aligned.
    const size_t node_bytes = ((size_t)(total_nodes + 1) * sizeof(int) + 63) / 64 * 64;
    const size_t child_bytes = ((size_t)(total_nodes + 1) * 4 * sizeof(int) + 63) / 64 * 64;
    node_start = (int *)aligned_alloc(64, node_bytes);
    node_end = (int *)aligned_alloc(64, node_bytes);
    node_suffix = (int *)aligned_alloc(64, node_bytes);
    node_children = (int *)aligned_alloc(64, child_bytes);
    if (!ref_dna || !node_start || !node_end || !node_suffix || !node_children) {
        fprintf(stderr, "[mummergpu] tree allocation failed (%d nodes)\n", total_nodes);
        exit(2);
    }

    memcpy(node_start, n_start.data(), (total_nodes + 1) * sizeof(int));
    memcpy(node_end, n_end.data(), (total_nodes + 1) * sizeof(int));
    memcpy(node_suffix, n_suffix.data(), (total_nodes + 1) * sizeof(int));
    memcpy(node_children, n_children.data(), (total_nodes + 1) * 4 * sizeof(int));

    std::vector<int> q_offsets;
    std::vector<int> q_lengths;
    std::string q_str = "";

    // Queries: every read in the shipped file (q25bp.50k.fna holds 50,000).
    // MUMMERGPU_QUERIES caps the count for a quick smoke run.
    long qry_cap = 0;
    if (const char *e = getenv("MUMMERGPU_QUERIES")) qry_cap = atol(e);

    if (qry_path) {
        std::ifstream qf(qry_path);
        std::string line;
        std::string curr = "";
        long count = 0;
        auto emit = [&]() {
            q_offsets.push_back((int)q_str.length());
            q_lengths.push_back((int)curr.length());
            q_str += 'q';
            q_str += curr;
            q_str += '\0';
            count++;
            curr.clear();
        };
        while (std::getline(qf, line)) {
            if (qry_cap > 0 && count >= qry_cap) break;
            if (line.empty()) continue;
            if (line[0] == '>') {
                if (!curr.empty()) emit();
            } else {
                for (char c : line) {
                    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) curr += c;
                }
            }
        }
        if (!curr.empty() && (qry_cap <= 0 || count < qry_cap)) emit();
    } else {
        // Draw the reads out of the reference so they actually match, the way
        // Rodinia's genreads.py builds the shipped query file.
        const long rbases = (long)ref_len - 2;   // skip the 's' prefix and '$'
        long n = qry_cap > 0 ? qry_cap : 50000;
        uint64_t st = 0xDEADBEEFCAFEF00Dull;
        for (long i = 0; i < n; i++) {
            st ^= st << 13; st ^= st >> 7; st ^= st << 17;
            long start = 1 + (long)(st % (uint64_t)(rbases - 25));
            q_offsets.push_back((int)q_str.length());
            q_lengths.push_back(25);
            q_str += 'q';
            q_str.append(ref_str, start, 25);
            q_str += '\0';
        }
    }

    num_queries = (int32_t)q_offsets.size();
    total_buf_size = q_str.length();

    query_offsets = (int *)aligned_alloc(64, (num_queries * sizeof(int) + 63) / 64 * 64);
    query_lengths = (int *)aligned_alloc(64, (num_queries * sizeof(int) + 63) / 64 * 64);
    memcpy(query_offsets, q_offsets.data(), num_queries * sizeof(int));
    memcpy(query_lengths, q_lengths.data(), num_queries * sizeof(int));

    queries_buf = (char *)aligned_alloc(64, (total_buf_size + 63) / 64 * 64);
    memcpy(queries_buf, q_str.data(), total_buf_size);

    mlir_res_node = (int *)aligned_alloc(64, (total_buf_size * sizeof(int) + 63) / 64 * 64);
    mlir_res_edge = (int *)aligned_alloc(64, (total_buf_size * sizeof(int) + 63) / 64 * 64);
    mlir_res_qry = (int *)aligned_alloc(64, (total_buf_size * sizeof(int) + 63) / 64 * 64);

    ref_res_node = (int *)aligned_alloc(64, (total_buf_size * sizeof(int) + 63) / 64 * 64);
    ref_res_edge = (int *)aligned_alloc(64, (total_buf_size * sizeof(int) + 63) / 64 * 64);
    ref_res_qry = (int *)aligned_alloc(64, (total_buf_size * sizeof(int) + 63) / 64 * 64);

    memset(mlir_res_node, 0, total_buf_size * sizeof(int));
    memset(mlir_res_edge, 0, total_buf_size * sizeof(int));
    memset(mlir_res_qry, 0, total_buf_size * sizeof(int));

    memset(ref_res_node, 0, total_buf_size * sizeof(int));
    memset(ref_res_edge, 0, total_buf_size * sizeof(int));
    memset(ref_res_qry, 0, total_buf_size * sizeof(int));
}

static void bench_call(void) {
    mummergpu_match(
        num_queries,
        MR1(query_offsets, num_queries),
        MR1(query_lengths, num_queries),
        MR1(queries_buf, total_buf_size),
        MR1(ref_dna, ref_len),
        min_match_len,
        MR1(node_start, total_nodes + 1),
        MR1(node_end, total_nodes + 1),
        MR1(node_suffix, total_nodes + 1),
        MR2(node_children, total_nodes + 1, 4),
        MR1(mlir_res_node, total_buf_size),
        MR1(mlir_res_edge, total_buf_size),
        MR1(mlir_res_qry, total_buf_size)
    );
}

static void bench_reference(void) {
    ref_mummergpu(
        num_queries,
        query_offsets,
        query_lengths,
        queries_buf,
        ref_dna,
        min_match_len,
        node_start,
        node_end,
        node_suffix,
        (const int (*)[4])node_children,
        ref_res_node,
        ref_res_edge,
        ref_res_qry
    );
}

// The dumps are relative to the repo root, which is not the cwd the runner
// uses, so a bare relative path silently fails to open and leaves a stale file
// behind.  Try the same candidate roots the dataset search uses.
static FILE *open_dump(const char *env, const char *name) {
    if (const char *p = getenv(env)) {
        FILE *f = fopen(p, "w");
        if (!f) fprintf(stderr, "[mummergpu] cannot write %s\n", p);
        return f;
    }
    const char *dirs[] = {
        "vendor/rodinia/extracted_mlir/drivers/",
        "drivers/",
        "./",
        "../../../vendor/rodinia/extracted_mlir/drivers/"
    };
    char path[512];
    for (auto d : dirs) {
        snprintf(path, sizeof(path), "%s%s", d, name);
        FILE *f = fopen(path, "w");
        if (f) return f;
    }
    fprintf(stderr, "[mummergpu] cannot write %s in any candidate directory\n", name);
    return nullptr;
}

static int bench_check(void) {
    int errs = 0;
    errs += dhir_compare_int("result_node", mlir_res_node, ref_res_node, total_buf_size);
    errs += dhir_compare_int("result_edge_len", mlir_res_edge, ref_res_edge, total_buf_size);
    errs += dhir_compare_int("result_qry_len", mlir_res_qry, ref_res_qry, total_buf_size);

    return errs;
}

static void bench_free(void) {
    free(ref_dna);
    free(node_start);
    free(node_end);
    free(node_suffix);
    free(node_children);
    free(query_offsets);
    free(query_lengths);
    free(queries_buf);
    free(mlir_res_node);
    free(mlir_res_edge);
    free(mlir_res_qry);
    free(ref_res_node);
    free(ref_res_edge);
    free(ref_res_qry);
}
