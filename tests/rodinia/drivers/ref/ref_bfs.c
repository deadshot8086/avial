#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_bfs_kernel(int no_of_nodes,
                    const int (*graph_nodes)[2],
                    const int *graph_edges,
                    int *graph_mask,
                    int *updating_graph_mask,
                    int *graph_visited,
                    int *cost) {
  int stop = 1;
  while (stop != 0) {
    stop = 0;
    for (int tid = 0; tid < no_of_nodes; tid++) {
      if (graph_mask[tid] != 0) {
        graph_mask[tid] = 0;
        int start = graph_nodes[tid][0];
        int edges = graph_nodes[tid][1];
        for (int i = start; i < start + edges; i++) {
          int id = graph_edges[i];
          if (graph_visited[id] == 0) {
            cost[id] = cost[tid] + 1;
            updating_graph_mask[id] = 1;
          }
        }
      }
    }

    for (int tid = 0; tid < no_of_nodes; tid++) {
      if (updating_graph_mask[tid] != 0) {
        graph_mask[tid] = 1;
        graph_visited[tid] = 1;
        stop = 1;
        updating_graph_mask[tid] = 0;
      }
    }
  }
}

#ifdef __cplusplus
}
#endif
