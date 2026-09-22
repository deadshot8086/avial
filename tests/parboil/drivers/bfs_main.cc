// MPI driver for the extracted Parboil bfs kernel.

#define DHIR_BENCH_NAME "bfs"
#include "dhir_bench.h"


extern "C" {
void bfs(int32_t, int32_t,
         int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t, // graph_nodes
         int32_t *, int32_t *, int64_t, int64_t, int64_t,   // graph_edges
         int32_t *, int32_t *, int64_t, int64_t, int64_t,   // color
         int32_t *, int32_t *, int64_t, int64_t, int64_t,   // cost
         int32_t *, int32_t *, int64_t, int64_t, int64_t);  // wavefront

void ref_bfs(int no_of_nodes, int source, const int *graph_nodes,
             const int *graph_edges, int *color, int *cost, int *wavefront);
}

#include <string>

static int no_of_nodes = 0;
static int edge_list_size = 0;
static int source = 0;

static int *graph_nodes = nullptr;  // [no_of_nodes][2] : {starting, no_of_edges}
static int *graph_edges = nullptr;  // [edge_list_size] : destination node id

static int *color = nullptr;        // kernel side
static int *cost = nullptr;
static int *wavefront = nullptr;

static int *color_ref = nullptr;    // reference side
static int *cost_ref = nullptr;
static int *wavefront_ref = nullptr;


static FILE *open_resolved(const char *path) {
    if (!path) return nullptr;
    FILE *fp = fopen(path, "r");
    if (fp) return fp;

    std::string p1 = std::string("vendor/parboil/datasets/bfs/UT/input/") + path;
    return fopen(p1.c_str(), "r");
}

static void drop_graph(void) {
    free(graph_nodes);
    free(graph_edges);
    graph_nodes = nullptr;
    graph_edges = nullptr;
    no_of_nodes = 0;
    edge_list_size = 0;
    source = 0;
}


static bool read_dataset(const char *path) {
    FILE *fp = open_resolved(path);
    if (!fp) return false;

    int n = 0;
    if (fscanf(fp, "%d", &n) != 1 || n <= 0) {
        fclose(fp);
        return false;
    }
    no_of_nodes = n;
    graph_nodes = dhir_alloc_int((long)n * 2);

    bool ok = true;
    for (int i = 0; i < n; i++) {
        int start = 0, edgeno = 0;
        if (fscanf(fp, "%d %d", &start, &edgeno) != 2) { ok = false; break; }
        graph_nodes[2 * i] = start;
        graph_nodes[2 * i + 1] = edgeno;
    }

    int m = 0;
    if (ok && (fscanf(fp, "%d", &source) != 1 ||
               fscanf(fp, "%d", &m) != 1 || m <= 0))
        ok = false;

    if (ok) {
        edge_list_size = m;
        graph_edges = dhir_alloc_int(m);
        for (int i = 0; i < m; i++) {
            int id = 0, weight = 0;
            if (fscanf(fp, "%d %d", &id, &weight) != 2) { ok = false; break; }
            graph_edges[i] = id;  // struct Edge::y (weight) is unused by BFS_CPU
        }
    }
    fclose(fp);

    // Structural validation: the traversal indexes graph_edges[start .. start+len)
    // and then color/cost[graph_edges[e]], so both have to stay in range.
    if (ok && (source < 0 || source >= no_of_nodes)) ok = false;
    for (int i = 0; ok && i < no_of_nodes; i++) {
        int start = graph_nodes[2 * i], len = graph_nodes[2 * i + 1];
        if (start < 0 || len < 0 || (long)start + len > edge_list_size) ok = false;
    }
    for (int e = 0; ok && e < edge_list_size; e++)
        if (graph_edges[e] < 0 || graph_edges[e] >= no_of_nodes) ok = false;

    if (!ok) {
        fprintf(stderr, "[bfs] %s is truncated or malformed, falling back to "
                        "synthetic graph\n", path);
        drop_graph();
        return false;
    }
    return true;
}


static void synth_graph(void) {
    int n = 4096, deg = 4;
    if (const char *e = getenv("BFS_NODES")) { int v = atoi(e); if (v > 1) n = v; }
    if (const char *e = getenv("BFS_DEGREE")) { int v = atoi(e); if (v > 0) deg = v; }

    no_of_nodes = n;
    edge_list_size = n * deg;
    source = 0;

    graph_nodes = dhir_alloc_int((long)n * 2);
    graph_edges = dhir_alloc_int((long)n * deg);

    srand(7);
    for (int i = 0; i < n; i++) {
        graph_nodes[2 * i] = i * deg;
        graph_nodes[2 * i + 1] = deg;
        graph_edges[i * deg] = (i + 1) % n;
        for (int k = 1; k < deg; k++)
            graph_edges[i * deg + k] = rand() % n;
    }

    fprintf(stderr, "[bfs] dataset not found, using synthetic graph "
                    "(nodes=%d degree=%d, srand(7))\n", n, deg);
}

static void bench_alloc(void) {
    const char *input = getenv("BFS_INPUT");
    if (!input) input = "vendor/parboil/datasets/bfs/UT/input/graph_input.dat";

    bool real_data = read_dataset(input);
    if (!real_data) synth_graph();

    color = dhir_alloc_int(no_of_nodes);
    cost = dhir_alloc_int(no_of_nodes);
    wavefront = dhir_alloc_int(no_of_nodes);
    color_ref = dhir_alloc_int(no_of_nodes);
    cost_ref = dhir_alloc_int(no_of_nodes);
    wavefront_ref = dhir_alloc_int(no_of_nodes);

    dhir_zero_int(color, no_of_nodes);
    dhir_zero_int(cost, no_of_nodes);
    dhir_zero_int(wavefront, no_of_nodes);
    dhir_zero_int(color_ref, no_of_nodes);
    dhir_zero_int(cost_ref, no_of_nodes);
    dhir_zero_int(wavefront_ref, no_of_nodes);

    printf("[bfs] %s nodes=%d edges=%d source=%d\n",
           real_data ? "dataset" : "synthetic", no_of_nodes, edge_list_size,
           source);
}

static void bench_call(void) {
    bfs(no_of_nodes, source,
        MR2(graph_nodes, no_of_nodes, 2),
        MR1(graph_edges, edge_list_size),
        MR1(color, no_of_nodes),
        MR1(cost, no_of_nodes),
        MR1(wavefront, no_of_nodes));
}

static void bench_reference(void) {
    ref_bfs(no_of_nodes, source, graph_nodes, graph_edges, color_ref, cost_ref,
            wavefront_ref);
}

static int bench_check(void) {
    int errors = dhir_compare_int("cost", cost, cost_ref, no_of_nodes);
    errors += dhir_compare_int("color", color, color_ref, no_of_nodes);
    errors += dhir_compare_int("wavefront", wavefront, wavefront_ref, no_of_nodes);
    errors += dhir_compare_int("wavefront", wavefront, wavefront_ref, no_of_nodes);
    return errors;
}

static void bench_free(void) {
    free(graph_nodes);
    free(graph_edges);
    free(color);
    free(cost);
    free(wavefront);
    free(color_ref);
    free(cost_ref);
    free(wavefront_ref);
    graph_nodes = graph_edges = nullptr;
    color = cost = wavefront = nullptr;
    color_ref = cost_ref = wavefront_ref = nullptr;
}
