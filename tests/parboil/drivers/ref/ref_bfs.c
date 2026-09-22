

#ifdef __cplusplus
extern "C" {
#endif

#define REF_BFS_INF 2147483647 /* 2^31-1 */
#define REF_BFS_WHITE 16677217
#define REF_BFS_GRAY 16677218
#define REF_BFS_BLACK 16677221

void ref_bfs(int no_of_nodes, int source,
             const int *graph_nodes,
             const int *graph_edges,
             int *color,
             int *cost,
             int *wavefront) {
  int i;
  int head, tail;

  for (i = 0; i < no_of_nodes; i++) {
    color[i] = REF_BFS_WHITE;
    cost[i] = REF_BFS_INF;
  }
  cost[source] = 0;

  head = 0;
  tail = 0;
  wavefront[tail] = source;
  tail++;
  color[source] = REF_BFS_GRAY;

  while (head < tail) {
    int index = wavefront[head];
    int first, last, e;
    head++;

    first = graph_nodes[2 * index];
    last = graph_nodes[2 * index + 1] + graph_nodes[2 * index];
    for (e = first; e < last; e++) {
      int id = graph_edges[e];
      if (color[id] == REF_BFS_WHITE) {
        cost[id] = cost[index] + 1;
        wavefront[tail] = id;
        tail++;
        color[id] = REF_BFS_GRAY;
      }
    }
    color[index] = REF_BFS_BLACK;
  }
}

#ifdef __cplusplus
}
#endif
