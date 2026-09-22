#ifndef B_PLUS_TREE_BUILDER_H
#define B_PLUS_TREE_BUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define DEFAULT_ORDER 508

typedef struct bpt_record {
    int value;
} bpt_record;

typedef struct bpt_node {
    void ** pointers;
    int * keys;
    struct bpt_node * parent;
    bool is_leaf;
    int num_keys;
    struct bpt_node * next;
} bpt_node;

typedef struct bpt_knode {
    int location;
    int indices[DEFAULT_ORDER + 1];
    int keys[DEFAULT_ORDER + 1];
    bool is_leaf;
    int num_keys;
} bpt_knode;

typedef struct bpt_tree_data {
    int size;               // Total records (1000000)
    int count;              // Query count from command.txt (10000)
    int maxheight;          // Tree height (2)
    int order;              // DEFAULT_ORDER (508)
    int order_width;        // DEFAULT_ORDER + 1 (509)
    int threadsPerBlock;    // 508
    long knodes_elem;       // Number of knodes (3953)
    bpt_knode * knodes;
    bpt_record * records;
    int * query_keys;
} bpt_tree_data;

namespace btree_internal {

static int order = DEFAULT_ORDER;
static bpt_node * queue = NULL;

static void enqueue(bpt_node * new_node) {
    if (queue == NULL) {
        queue = new_node;
        queue->next = NULL;
    } else {
        bpt_node * c = queue;
        while (c->next != NULL) c = c->next;
        c->next = new_node;
        new_node->next = NULL;
    }
}

static bpt_node * dequeue(void) {
    bpt_node * n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}

static int height(bpt_node * root) {
    int h = 0;
    bpt_node * c = root;
    while (!c->is_leaf) {
        c = (bpt_node *) c->pointers[0];
        h++;
    }
    return h;
}

static int cut(int length) {
    if (length % 2 == 0) return length / 2;
    else return length / 2 + 1;
}

static bpt_record * make_record(int value) {
    bpt_record * new_record = (bpt_record *)malloc(sizeof(bpt_record));
    if (!new_record) { perror("Record creation"); exit(1); }
    new_record->value = value;
    return new_record;
}

static bpt_node * make_node(void) {
    bpt_node * new_node = (bpt_node *)malloc(sizeof(bpt_node));
    if (!new_node) { perror("Node creation"); exit(1); }
    new_node->keys = (int *)malloc((order - 1) * sizeof(int));
    new_node->pointers = (void **)malloc(order * sizeof(void *));
    new_node->is_leaf = false;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}

static bpt_node * make_leaf(void) {
    bpt_node * leaf = make_node();
    leaf->is_leaf = true;
    return leaf;
}

static bpt_node * find_leaf(bpt_node * root, int key) {
    int i = 0;
    bpt_node * c = root;
    if (c == NULL) return c;
    while (!c->is_leaf) {
        i = 0;
        while (i < c->num_keys) {
            if (key >= c->keys[i]) i++;
            else break;
        }
        c = (bpt_node *)c->pointers[i];
    }
    return c;
}

static bpt_record * find(bpt_node * root, int key) {
    int i = 0;
    bpt_node * c = find_leaf(root, key);
    if (c == NULL) return NULL;
    for (i = 0; i < c->num_keys; i++) {
        if (c->keys[i] == key) break;
    }
    if (i == c->num_keys) return NULL;
    else return (bpt_record *)c->pointers[i];
}

static int get_left_index(bpt_node * parent, bpt_node * left) {
    int left_index = 0;
    while (left_index <= parent->num_keys && parent->pointers[left_index] != left)
        left_index++;
    return left_index;
}

static bpt_node * insert_into_leaf(bpt_node * leaf, int key, bpt_record * pointer) {
    int i, insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
        insertion_point++;
    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }
    leaf->keys[insertion_point] = key;
    leaf->pointers[insertion_point] = pointer;
    leaf->num_keys++;
    return leaf;
}

static bpt_node * insert_into_parent(bpt_node * root, bpt_node * left, int key, bpt_node * right);

static bpt_node * insert_into_leaf_after_splitting(bpt_node * root, bpt_node * leaf, int key, bpt_record * pointer) {
    bpt_node * new_leaf = make_leaf();
    int * temp_keys = (int *)malloc(order * sizeof(int));
    void ** temp_pointers = (void **)malloc(order * sizeof(void *));
    int insertion_index = 0;
    while (leaf->keys[insertion_index] < key && insertion_index < order - 1)
        insertion_index++;
    int i, j;
    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->keys[i];
        temp_pointers[j] = leaf->pointers[i];
    }
    temp_keys[insertion_index] = key;
    temp_pointers[insertion_index] = pointer;
    leaf->num_keys = 0;
    int split = cut(order - 1);
    for (i = 0; i < split; i++) {
        leaf->pointers[i] = temp_pointers[i];
        leaf->keys[i] = temp_keys[i];
        leaf->num_keys++;
    }
    for (i = split, j = 0; i < order; i++, j++) {
        new_leaf->pointers[j] = temp_pointers[i];
        new_leaf->keys[j] = temp_keys[i];
        new_leaf->num_keys++;
    }
    free(temp_pointers);
    free(temp_keys);
    new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
    leaf->pointers[order - 1] = new_leaf;
    for (i = leaf->num_keys; i < order - 1; i++) leaf->pointers[i] = NULL;
    for (i = new_leaf->num_keys; i < order - 1; i++) new_leaf->pointers[i] = NULL;
    new_leaf->parent = leaf->parent;
    int new_key = new_leaf->keys[0];
    return insert_into_parent(root, leaf, new_key, new_leaf);
}

static bpt_node * insert_into_node(bpt_node * root, bpt_node * n, int left_index, int key, bpt_node * right) {
    int i;
    for (i = n->num_keys; i > left_index; i--) {
        n->pointers[i + 1] = n->pointers[i];
        n->keys[i] = n->keys[i - 1];
    }
    n->pointers[left_index + 1] = right;
    n->keys[left_index] = key;
    n->num_keys++;
    return root;
}

static bpt_node * insert_into_node_after_splitting(bpt_node * root, bpt_node * old_node, int left_index, int key, bpt_node * right) {
    int i, j, split, k_prime;
    bpt_node * new_node, * child;
    bpt_node ** temp_pointers = (bpt_node **)malloc((order + 1) * sizeof(bpt_node *));
    int * temp_keys = (int *)malloc(order * sizeof(int));
    for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pointers[j] = (bpt_node *)old_node->pointers[i];
    }
    for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = old_node->keys[i];
    }
    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;
    split = cut(order);
    new_node = make_node();
    old_node->num_keys = 0;
    for (i = 0; i < split - 1; i++) {
        old_node->pointers[i] = temp_pointers[i];
        old_node->keys[i] = temp_keys[i];
        old_node->num_keys++;
    }
    old_node->pointers[i] = temp_pointers[i];
    k_prime = temp_keys[split - 1];
    for (++i, j = 0; i < order; i++, j++) {
        new_node->pointers[j] = temp_pointers[i];
        new_node->keys[j] = temp_keys[i];
        new_node->num_keys++;
    }
    new_node->pointers[j] = temp_pointers[i];
    free(temp_pointers);
    free(temp_keys);
    new_node->parent = old_node->parent;
    for (i = 0; i <= new_node->num_keys; i++) {
        child = (bpt_node *)new_node->pointers[i];
        child->parent = new_node;
    }
    return insert_into_parent(root, old_node, k_prime, new_node);
}

static bpt_node * insert_into_new_root(bpt_node * left, int key, bpt_node * right) {
    bpt_node * root = make_node();
    root->keys[0] = key;
    root->pointers[0] = left;
    root->pointers[1] = right;
    root->num_keys++;
    root->parent = NULL;
    left->parent = root;
    right->parent = root;
    return root;
}

static bpt_node * insert_into_parent(bpt_node * root, bpt_node * left, int key, bpt_node * right) {
    bpt_node * parent = left->parent;
    if (parent == NULL) return insert_into_new_root(left, key, right);
    int left_index = get_left_index(parent, left);
    if (parent->num_keys < order - 1)
        return insert_into_node(root, parent, left_index, key, right);
    return insert_into_node_after_splitting(root, parent, left_index, key, right);
}

static bpt_node * start_new_tree(int key, bpt_record * pointer) {
    bpt_node * root = make_leaf();
    root->keys[0] = key;
    root->pointers[0] = pointer;
    root->pointers[order - 1] = NULL;
    root->parent = NULL;
    root->num_keys++;
    return root;
}

static bpt_node * insert(bpt_node * root, int key, int value) {
    if (find(root, key) != NULL) return root;
    bpt_record * pointer = make_record(value);
    if (root == NULL) return start_new_tree(key, pointer);
    bpt_node * leaf = find_leaf(root, key);
    if (leaf->num_keys < order - 1) {
        insert_into_leaf(leaf, key, pointer);
        return root;
    }
    return insert_into_leaf_after_splitting(root, leaf, key, pointer);
}

static void destroy_tree_nodes(bpt_node * root) {
    if (root == NULL) return;
    if (root->is_leaf) {
        for (int i = 0; i < root->num_keys; i++) {
            free(root->pointers[i]);
        }
    } else {
        for (int i = 0; i <= root->num_keys; i++) {
            destroy_tree_nodes((bpt_node *)root->pointers[i]);
        }
    }
    free(root->keys);
    free(root->pointers);
    free(root);
}

} // namespace btree_internal

static inline bool load_rodinia_btree(const char *input_file, const char *command_file, bpt_tree_data *data) {
    // 1. Process command.txt first to get query count and align rand() calls
    FILE *cf = fopen(command_file, "r");
    if (!cf) {
        fprintf(stderr, "Cannot open command file: %s\n", command_file);
        return false;
    }
    char cmd;
    int count = 0;
    int skipped_rands = 0;
    while (fscanf(cf, " %c", &cmd) == 1) {
        if (cmd == 'j') {
            int j_count = 0, rSize = 0;
            if (fscanf(cf, "%d %d", &j_count, &rSize) == 2) {
                skipped_rands += j_count;
            }
        } else if (cmd == 'k') {
            if (fscanf(cf, "%d", &count) == 1) {
                break;
            }
        } else {
            char line[256];
            if (!fgets(line, sizeof(line), cf)) break;
        }
    }
    fclose(cf);

    if (count <= 0) {
        fprintf(stderr, "No command 'k' found in %s\n", command_file);
        return false;
    }

    // 2. Read input file (mil.txt) and build B+ tree
    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open input file: %s\n", input_file);
        return false;
    }
    int size = 0;
    if (fscanf(fp, "%d", &size) != 1 || size <= 0) {
        fprintf(stderr, "Invalid input size in %s\n", input_file);
        fclose(fp);
        return false;
    }

    bpt_node * root = NULL;
    int val = 0;
    while (fscanf(fp, "%d", &val) == 1) {
        root = btree_internal::insert(root, val, val);
    }
    fclose(fp);

    int maxh = btree_internal::height(root);

    // 3. Transform to flat representation (matching transform_to_cuda)
    long max_nodes = (long)(pow(DEFAULT_ORDER, log(size) / log(DEFAULT_ORDER / 2.0) - 1) + 1);
    bpt_record * krecords = (bpt_record *)malloc(size * sizeof(bpt_record));
    bpt_knode * knodes = (bpt_knode *)malloc(max_nodes * sizeof(bpt_knode));

    btree_internal::queue = NULL;
    btree_internal::enqueue(root);
    bpt_node * n;
    bpt_knode * k;
    int i;
    long nodeindex = 0;
    long recordindex = 0;
    long queueindex = 0;
    knodes[0].location = nodeindex++;

    while (btree_internal::queue != NULL) {
        n = btree_internal::dequeue();
        k = &knodes[queueindex];
        k->location = queueindex++;
        k->is_leaf = n->is_leaf;
        k->num_keys = n->num_keys + 2;
        k->keys[0] = INT_MIN;
        k->keys[k->num_keys - 1] = INT_MAX;
        for (i = k->num_keys; i < DEFAULT_ORDER; i++) k->keys[i] = INT_MAX;
        if (!k->is_leaf) {
            k->indices[0] = nodeindex++;
            for (i = 1; i < k->num_keys - 1; i++) {
                k->keys[i] = n->keys[i - 1];
                btree_internal::enqueue((bpt_node *)n->pointers[i - 1]);
                k->indices[i] = nodeindex++;
            }
            btree_internal::enqueue((bpt_node *)n->pointers[i - 1]);
        } else {
            k->indices[0] = 0;
            for (i = 1; i < k->num_keys - 1; i++) {
                k->keys[i] = n->keys[i - 1];
                krecords[recordindex].value = ((bpt_record *)n->pointers[i - 1])->value;
                k->indices[i] = recordindex++;
            }
        }
        k->indices[k->num_keys - 1] = queueindex;
    }

    // Free original pointer tree
    btree_internal::destroy_tree_nodes(root);

    // 4. Generate query keys matching Rodinia 3.1 OpenMP PRNG state
    srand(1);
    for (int s = 0; s < skipped_rands; s++) {
        rand();
    }
    int *query_keys = (int *)malloc(count * sizeof(int));
    for (int q = 0; q < count; q++) {
        query_keys[q] = (int)((rand() / (float)RAND_MAX) * size);
    }

    data->size = size;
    data->count = count;
    data->maxheight = maxh;
    data->order = DEFAULT_ORDER;
    data->order_width = DEFAULT_ORDER + 1;
    data->threadsPerBlock = DEFAULT_ORDER;
    data->knodes_elem = nodeindex;
    data->knodes = knodes;
    data->records = krecords;
    data->query_keys = query_keys;

    return true;
}

static inline void free_rodinia_btree(bpt_tree_data *data) {
    if (data->knodes) free(data->knodes);
    if (data->records) free(data->records);
    if (data->query_keys) free(data->query_keys);
}

#endif // B_PLUS_TREE_BUILDER_H
