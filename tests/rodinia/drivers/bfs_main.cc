#define DHIR_BENCH_NAME "bfs"
#include "dhir_bench.h"

extern "C" {
void bfs_kernel(int32_t,
                int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
                int32_t *, int32_t *, int64_t, int64_t, int64_t,
                int32_t *, int32_t *, int64_t, int64_t, int64_t,
                int32_t *, int32_t *, int64_t, int64_t, int64_t,
                int32_t *, int32_t *, int64_t, int64_t, int64_t,
                int32_t *, int32_t *, int64_t, int64_t, int64_t);

void ref_bfs_kernel(int no_of_nodes,
                    const int (*graph_nodes)[2],
                    const int *graph_edges,
                    int *graph_mask,
                    int *updating_graph_mask,
                    int *graph_visited,
                    int *cost);
}

static int no_of_nodes = 0;
static int edge_list_size = 0;
static int (*graph_nodes)[2];
static int *graph_edges;

static int *graph_mask, *updating_graph_mask, *graph_visited, *cost;
static int *graph_mask_ref, *updating_graph_mask_ref, *graph_visited_ref, *cost_ref;
static int *graph_mask_init, *graph_visited_init, *cost_init;

static void read_graph(const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open graph file: %s\n", file);
        exit(1);
    }
    if (fscanf(fp, "%d", &no_of_nodes) != 1) exit(1);

    graph_nodes = (int (*)[2])malloc(sizeof(int) * 2 * no_of_nodes);
    graph_mask_init = dhir_alloc_int(no_of_nodes);
    graph_visited_init = dhir_alloc_int(no_of_nodes);
    cost_init = dhir_alloc_int(no_of_nodes);

    for (int i = 0; i < no_of_nodes; i++) {
        int start, edgeno;
        if (fscanf(fp, "%d %d", &start, &edgeno) != 2) exit(1);
        graph_nodes[i][0] = start;
        graph_nodes[i][1] = edgeno;
        graph_mask_init[i] = 0;
        graph_visited_init[i] = 0;
        cost_init[i] = -1;
    }

    int source = 0;
    if (fscanf(fp, "%d", &source) != 1) exit(1);
    graph_mask_init[source] = 1;
    graph_visited_init[source] = 1;
    cost_init[source] = 0;

    if (fscanf(fp, "%d", &edge_list_size) != 1) exit(1);
    graph_edges = (int *)malloc(sizeof(int) * edge_list_size);
    for (int i = 0; i < edge_list_size; i++) {
        int id, cost_val;
        if (fscanf(fp, "%d %d", &id, &cost_val) != 2) exit(1);
        graph_edges[i] = id;
    }
    fclose(fp);
}

static void bench_alloc(void) {
    read_graph("vendor/rodinia/rodinia_3.1/data/bfs/graph1MW_6.txt");

    graph_mask = dhir_alloc_int(no_of_nodes);
    updating_graph_mask = dhir_alloc_int(no_of_nodes);
    graph_visited = dhir_alloc_int(no_of_nodes);
    cost = dhir_alloc_int(no_of_nodes);

    graph_mask_ref = dhir_alloc_int(no_of_nodes);
    updating_graph_mask_ref = dhir_alloc_int(no_of_nodes);
    graph_visited_ref = dhir_alloc_int(no_of_nodes);
    cost_ref = dhir_alloc_int(no_of_nodes);

    for (int i = 0; i < no_of_nodes; i++) {
        graph_mask[i] = graph_mask_init[i];
        graph_mask_ref[i] = graph_mask_init[i];
        graph_visited[i] = graph_visited_init[i];
        graph_visited_ref[i] = graph_visited_init[i];
        cost[i] = cost_init[i];
        cost_ref[i] = cost_init[i];
        updating_graph_mask[i] = 0;
        updating_graph_mask_ref[i] = 0;
    }
}

static void bench_call(void) {
    bfs_kernel(no_of_nodes,
               MR2((int *)graph_nodes, no_of_nodes, 2),
               MR1(graph_edges, edge_list_size),
               MR1(graph_mask, no_of_nodes),
               MR1(updating_graph_mask, no_of_nodes),
               MR1(graph_visited, no_of_nodes),
               MR1(cost, no_of_nodes));
}

static void bench_reference(void) {
    ref_bfs_kernel(no_of_nodes, graph_nodes, graph_edges,
                   graph_mask_ref, updating_graph_mask_ref,
                   graph_visited_ref, cost_ref);
}

static int bench_check(void) {
    int errors = dhir_compare_int("cost", cost, cost_ref, no_of_nodes);
    errors += dhir_compare_int("mask", graph_mask, graph_mask_ref, no_of_nodes);
    return errors;
}

static void bench_free(void) {
    free(graph_nodes);
    free(graph_edges);
    free(graph_mask); free(updating_graph_mask); free(graph_visited); free(cost);
    free(graph_mask_ref); free(updating_graph_mask_ref); free(graph_visited_ref); free(cost_ref);
    free(graph_mask_init); free(graph_visited_init); free(cost_init);
}
