// Reference implementation for Rodinia mummergpu benchmark
#include <stdint.h>

void ref_mummergpu(int num_queries,
                   const int query_offsets[],
                   const int query_lengths[],
                   const char queries[],
                   const char ref[],
                   int min_match_len,
                   const int node_start[],
                   const int node_end[],
                   const int node_suffix[],
                   const int node_children[][4],
                   int result_node[],
                   int result_edge_len[],
                   int result_qry_len[]) {
    for (int qryid = 0; qryid < num_queries; qryid++) {
        int qry_start_offset = query_offsets[qryid];
        int qlen = query_lengths[qryid];
        int last = qlen - min_match_len;

        int cur = 1;
        int mustmatch = 0;
        int qry_match_len = 0;

        for (int qrystart = 0; qrystart <= last; qrystart++) {
            if (cur == 0 || qry_match_len < 1) {
                cur = 1;
                qry_match_len = 1;
                mustmatch = 0;
            }

            int prev = cur;
            char c = queries[qry_start_offset + qrystart + qry_match_len];
            int refpos = 0;

            int mismatch = 0;
            int no_edge = 0;

            while (c != '\0') {
                int child_idx = -1;
                if (c == 'A' || c == 'a') child_idx = 0;
                else if (c == 'C' || c == 'c') child_idx = 1;
                else if (c == 'G' || c == 'g') child_idx = 2;
                else if (c == 'T' || c == 't') child_idx = 3;

                int next_node = (child_idx >= 0) ? node_children[cur][child_idx] : 0;
                prev = cur;
                cur = next_node;

                if (cur == 0) {
                    no_edge = 1;
                    break;
                }

                int n_start = node_start[cur];
                int n_end = node_end[cur];
                int edgelen = n_end - n_start + 1;

                if (mustmatch > 0) {
                    int dist_to_edge_end = mustmatch - edgelen;
                    int edge_matchlen = n_start + mustmatch;
                    int past_node_end = n_end + 1;
                    refpos = (edge_matchlen < past_node_end) ? edge_matchlen : past_node_end;
                    qry_match_len += (edgelen < mustmatch) ? edgelen : mustmatch;
                    mustmatch = (dist_to_edge_end > 0) ? dist_to_edge_end : 0;
                } else {
                    qry_match_len++;
                    refpos = n_start + 1;
                }

                c = queries[qry_start_offset + qrystart + qry_match_len];

                while (refpos <= n_end && c != '\0') {
                    char r = ref[refpos];
                    if (r != c) {
                        mismatch = 1;
                        break;
                    }
                    qry_match_len++;
                    refpos++;
                    c = queries[qry_start_offset + qrystart + qry_match_len];
                }

                if (mismatch) {
                    break;
                }
            }

            if (no_edge) {
                if (qry_match_len > min_match_len) {
                    int out_idx = qry_start_offset + qrystart;
                    result_node[out_idx] = prev;
                    result_edge_len[out_idx] = 0;
                    result_qry_len[out_idx] = qry_match_len;
                }
                qry_match_len -= 1;
                mustmatch = 0;
                cur = node_suffix[prev];
            } else {
                if (qry_match_len > min_match_len) {
                    int out_idx = qry_start_offset + qrystart;
                    result_node[out_idx] = cur;
                    result_edge_len[out_idx] = refpos - node_start[cur];
                    result_qry_len[out_idx] = qry_match_len;
                }
                mustmatch = refpos - node_start[cur];
                qry_match_len -= (mustmatch + 1);
                cur = node_suffix[prev];
            }
        }
    }
}
