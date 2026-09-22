#define DHIR_BENCH_NAME "b_plus_tree"
#include "dhir_bench.h"
#include "ref/b_plus_tree_builder.h"

extern "C" {
void b_plus_tree(int32_t, int32_t, int32_t, int64_t,
                 int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
                 int32_t *, int32_t *, int64_t, int64_t, int64_t, int64_t, int64_t,
                 int32_t *, int32_t *, int64_t, int64_t, int64_t,
                 int32_t *, int32_t *, int64_t, int64_t, int64_t,
                 int64_t *, int64_t *, int64_t, int64_t, int64_t,
                 int64_t *, int64_t *, int64_t, int64_t, int64_t,
                 int32_t *, int32_t *, int64_t, int64_t, int64_t);

void ref_b_plus_tree(int count, int maxheight, int order,
                     long knodes_elem,
                     const int *knodes_keys,
                     const int *knodes_indices,
                     const int *records_val,
                     const int *keys,
                     long *currKnode,
                     long *offset,
                     int *ans_val);
}

static int count = 0;
static int maxheight = 0;
static int order = 0;
static int order_width = 0;
static long knodes_elem = 0;
static long total_records = 0;

static int *knodes_keys, *knodes_indices;
static int *records_val;
static int *keys;
static int64_t *currKnode, *currKnode_ref, *currKnode_init;
static int64_t *offset, *offset_ref, *offset_init;
static int *ans_val, *ans_ref;

static void bench_alloc(void) {
    bpt_tree_data tree_data;
    const char *data_file = "vendor/rodinia/rodinia_3.1/data/b+tree/mil.txt";
    const char *cmd_file = "vendor/rodinia/rodinia_3.1/data/b+tree/command.txt";

    if (!load_rodinia_btree(data_file, cmd_file, &tree_data)) {
        fprintf(stderr, "Failed to load Rodinia 3.1 B+ tree datasets!\n");
        exit(1);
    }

    count = tree_data.count;
    maxheight = tree_data.maxheight;
    order = tree_data.order;
    order_width = tree_data.order_width;
    knodes_elem = tree_data.knodes_elem;
    total_records = tree_data.size;

    knodes_keys = dhir_alloc_int(knodes_elem * order_width);
    knodes_indices = dhir_alloc_int(knodes_elem * order_width);
    records_val = dhir_alloc_int(total_records);
    keys = dhir_alloc_int(count);

    currKnode = (int64_t *)dhir_alloc_int64(count);
    currKnode_ref = (int64_t *)dhir_alloc_int64(count);
    currKnode_init = (int64_t *)dhir_alloc_int64(count);

    offset = (int64_t *)dhir_alloc_int64(count);
    offset_ref = (int64_t *)dhir_alloc_int64(count);
    offset_init = (int64_t *)dhir_alloc_int64(count);

    ans_val = dhir_alloc_int(count);
    ans_ref = dhir_alloc_int(count);

    for (long k = 0; k < knodes_elem; k++) {
        for (int j = 0; j < order_width; j++) {
            knodes_keys[k * order_width + j] = tree_data.knodes[k].keys[j];
            knodes_indices[k * order_width + j] = tree_data.knodes[k].indices[j];
        }
    }
    for (long r = 0; r < total_records; r++) {
        records_val[r] = tree_data.records[r].value;
    }

    for (int i = 0; i < count; i++) {
        keys[i] = tree_data.query_keys[i];
        currKnode_init[i] = 0;
        offset_init[i] = 0;
        currKnode[i] = 0;
        currKnode_ref[i] = 0;
        offset[i] = 0;
        offset_ref[i] = 0;
        ans_val[i] = -1;
        ans_ref[i] = -1;
    }

    free_rodinia_btree(&tree_data);
}

static void bench_call(void) {
    b_plus_tree(count, maxheight, order, knodes_elem,
                MR2(knodes_keys, knodes_elem, order_width),
                MR2(knodes_indices, knodes_elem, order_width),
                MR1(records_val, total_records),
                MR1(keys, count),
                MR1(currKnode, count),
                MR1(offset, count),
                MR1(ans_val, count));
}

static void bench_reference(void) {
    ref_b_plus_tree(count, maxheight, order, knodes_elem,
                    knodes_keys, knodes_indices, records_val, keys,
                    (long *)currKnode_ref, (long *)offset_ref, ans_ref);
}

static int bench_check(void) {
    int errors = dhir_compare_int("ans_val", ans_val, ans_ref, count);
    errors += dhir_compare_int64("currKnode", currKnode, currKnode_ref, count);
    errors += dhir_compare_int64("offset", offset, offset_ref, count);
    return errors;
}

static void bench_free(void) {
    free(knodes_keys); free(knodes_indices);
    free(records_val); free(keys);
    free(currKnode); free(currKnode_ref); free(currKnode_init);
    free(offset); free(offset_ref); free(offset_init);
    free(ans_val); free(ans_ref);
}
