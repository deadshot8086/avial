#define DHIR_BENCH_NAME "huffman"
#include "dhir_bench.h"
#include <vector>
#include <queue>
#include <map>
#include <cmath>

extern "C" void cpu_vlc_encode(
    uint32_t *, uint32_t *, int64_t, int64_t, int64_t,
    int32_t,
    uint32_t *, uint32_t *, int64_t, int64_t, int64_t,
    uint32_t *, uint32_t *, int64_t, int64_t, int64_t,
    uint32_t *, uint32_t *, int64_t, int64_t, int64_t, int64_t, int64_t
);

extern "C" void ref_huffman(
    const uint32_t indata[], uint32_t num_elements,
    uint32_t outdata[], uint32_t outsize[],
    const uint32_t codebook[][2]
);

static uint32_t *indata = nullptr;
static uint32_t *codebook = nullptr;
static uint32_t *outdata_mlir = nullptr;
static uint32_t *outdata_ref = nullptr;
static uint32_t *outsize_mlir = nullptr;
static uint32_t *outsize_ref = nullptr;

static int32_t num_elements = 0;
static size_t outdata_cap = 0;

struct INode {
    int f;
    virtual ~INode() {}
    INode(int f) : f(f) {}
};

struct InternalNode : public INode {
    INode *left;
    INode *right;
    InternalNode(INode *c0, INode *c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
    ~InternalNode() {
        delete left;
        delete right;
    }
};

struct LeafNode : public INode {
    unsigned char c;
    LeafNode(int f, unsigned char c) : INode(f), c(c) {}
};

struct NodeCmp {
    bool operator()(const INode *lhs, const INode *rhs) const { return lhs->f > rhs->f; }
};

static void GenerateCodes(const INode *node, std::vector<bool> &prefix, uint32_t cb[256][2]) {
    if (const LeafNode *lf = dynamic_cast<const LeafNode *>(node)) {
        uint32_t cw = 0;
        uint32_t count = (uint32_t)prefix.size();
        for (size_t i = 0; i < count; i++) {
            if (prefix[i]) cw += (1U << (count - i - 1));
        }
        cb[lf->c][0] = cw;
        cb[lf->c][1] = count;
    } else if (const InternalNode *in = dynamic_cast<const InternalNode *>(node)) {
        prefix.push_back(false);
        GenerateCodes(in->left, prefix, cb);
        prefix.pop_back();

        prefix.push_back(true);
        GenerateCodes(in->right, prefix, cb);
        prefix.pop_back();
    }
}

static void build_codebook(const unsigned char *data, size_t nbytes, uint32_t cb[256][2]) {
    unsigned int freqs[256] = {0};
    for (size_t i = 0; i < nbytes; i++) {
        freqs[data[i]]++;
    }

    std::priority_queue<INode *, std::vector<INode *>, NodeCmp> trees;
    for (int i = 0; i < 256; i++) {
        if (freqs[i] > 0) {
            trees.push(new LeafNode(freqs[i], (unsigned char)i));
        }
    }

    if (trees.empty()) {
        for (int i = 0; i < 256; i++) {
            trees.push(new LeafNode(1, (unsigned char)i));
        }
    } else if (trees.size() == 1) {
        trees.push(new LeafNode(1, (trees.top() == nullptr) ? 0 : 1));
    }

    while (trees.size() > 1) {
        INode *childR = trees.top(); trees.pop();
        INode *childL = trees.top(); trees.pop();
        trees.push(new InternalNode(childR, childL));
    }

    INode *root = trees.top();
    std::vector<bool> prefix;
    GenerateCodes(root, prefix, cb);
    delete root;
}

static void bench_alloc(void) {
    const char *data_path = getenv("HUFFMAN_INPUT");
    if (!data_path) data_path = getenv("HUFFMAN_DATASET");
    if (!data_path) {
        const char *candidates[] = {
            "vendor/rodinia/rodinia_3.1/data/huffman/test1024_H2.206587175259.in",
        };
        for (auto c : candidates) {
            FILE *f = fopen(c, "rb");
            if (f) {
                fclose(f);
                data_path = c;
                break;
            }
        }
    }

    size_t file_bytes = 0;
    if (data_path) {
        FILE *f = fopen(data_path, "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            file_bytes = ftell(f);
            fclose(f);
        }
    }

    if (file_bytes > 0) {
        num_elements = (int32_t)(file_bytes / sizeof(uint32_t));
    } else {
        num_elements = 65536;
        file_bytes = num_elements * sizeof(uint32_t);
    }

    indata = (uint32_t *)aligned_alloc(64, num_elements * sizeof(uint32_t));
    if (data_path) {
        FILE *f = fopen(data_path, "rb");
        size_t read_bytes = fread(indata, 1, num_elements * sizeof(uint32_t), f);
        fclose(f);
        (void)read_bytes;
    } else {
        for (int32_t i = 0; i < num_elements; i++) {
            indata[i] = (uint32_t)(i * 1103515245 + 12345);
        }
    }

    codebook = (uint32_t *)aligned_alloc(64, 256 * 2 * sizeof(uint32_t));
    memset(codebook, 0, 256 * 2 * sizeof(uint32_t));
    uint32_t (*cb_2d)[2] = (uint32_t (*)[2])codebook;
    build_codebook((const unsigned char *)indata, num_elements * sizeof(uint32_t), cb_2d);

    outdata_cap = (size_t)num_elements * 2 + 1024;
    outdata_mlir = (uint32_t *)aligned_alloc(64, outdata_cap * sizeof(uint32_t));
    outdata_ref = (uint32_t *)aligned_alloc(64, outdata_cap * sizeof(uint32_t));
    memset(outdata_mlir, 0, outdata_cap * sizeof(uint32_t));
    memset(outdata_ref, 0, outdata_cap * sizeof(uint32_t));

    outsize_mlir = (uint32_t *)aligned_alloc(64, 16 * sizeof(uint32_t));
    outsize_ref = (uint32_t *)aligned_alloc(64, 16 * sizeof(uint32_t));
    memset(outsize_mlir, 0, 16 * sizeof(uint32_t));
    memset(outsize_ref, 0, 16 * sizeof(uint32_t));
}

static void bench_call(void) {
    cpu_vlc_encode(
        MR1(indata, num_elements),
        num_elements,
        MR1(outdata_mlir, outdata_cap),
        MR1(outsize_mlir, 16),
        MR2(codebook, 256, 2)
    );
}

static void bench_reference(void) {
    const uint32_t (*cb_2d)[2] = (const uint32_t (*)[2])codebook;
    ref_huffman(indata, (uint32_t)num_elements, outdata_ref, outsize_ref, cb_2d);
}

static int bench_check(void) {
    int errs = 0;
    // Check outsize: [0]=totalBytes, [1]=startbit, [2]=word_idx
    errs += dhir_compare_int("outsize", (const int *)outsize_mlir, (const int *)outsize_ref, 3);

    // Check words in outdata
    uint32_t words_written = outsize_ref[2] + 1;
    if (words_written > outdata_cap) words_written = outdata_cap;
    errs += dhir_compare_int("outdata", (const int *)outdata_mlir, (const int *)outdata_ref, words_written);

    return errs;
}

static void bench_free(void) {
    free(indata);
    free(codebook);
    free(outdata_mlir);
    free(outdata_ref);
    free(outsize_mlir);
    free(outsize_ref);
}
