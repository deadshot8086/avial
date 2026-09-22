#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

inline unsigned char b2i(char base) {
    switch (base) {
        case 'A': case 'a': return 0;
        case 'C': case 'c': return 1;
        case 'G': case 'g': return 2;
        case 'T': case 't': return 3;
        case '$': return 4;
        default: return 0;
    }
}

class SuffixNode {
public:
    static int s_nodecount;
    int m_start;
    int m_end;
    int m_nodeid;
    int m_leafid;
    int m_depth;
    int m_numleaves;
    SuffixNode *m_children[5];
    SuffixNode *m_parent;
    SuffixNode *m_suffix;
    SuffixNode *m_printParent;

    SuffixNode(int s, int e, int leafid, SuffixNode *p, SuffixNode *x)
        : m_start(s), m_end(e), m_nodeid(++s_nodecount), m_leafid(leafid),
          m_numleaves(0), m_parent(p), m_suffix(x), m_printParent(nullptr) {
        for (int i = 0; i < 5; i++) m_children[i] = nullptr;
        m_depth = len();
        if (p) m_depth += p->m_depth;
    }

    ~SuffixNode() {
        for (int i = 0; i < 5; i++) {
            if (m_children[i]) delete m_children[i];
        }
    }

    int id() const {
        return this ? m_nodeid : 0;
    }

    int len(int i = -1) const {
        if (i != -1 && i < m_end) return i - m_start + 1;
        return m_end - m_start + 1;
    }
};

int SuffixNode::s_nodecount = 0;

class SuffixTree {
public:
    SuffixNode *m_root;
    const char *m_string;
    int m_strlen;

    SuffixTree(const char *s) : m_string(s) {
        m_strlen = (int)strlen(s);
        m_root = new SuffixNode(0, 0, 0, nullptr, nullptr);
        m_root->m_suffix = m_root;
    }

    ~SuffixTree() {
        delete m_root;
    }

    void buildUkkonen() {
        int len = m_strlen - 1; // remove 's'
        char base = m_string[1];

        SuffixNode *node = new SuffixNode(1, len, 1, m_root, nullptr);
        m_root->m_children[b2i(base)] = node;
        SuffixNode *lastleaf = node;

        int startj = 2;

        for (int i = 2; i <= len; i++) {
            node = lastleaf;
            int nodewalk = 0;
            SuffixNode *splitnode = nullptr;

            for (int j = startj; j <= i; j++) {
                int betapos = i;

                if (node != m_root) {
                    if (nodewalk) {
                        betapos -= nodewalk - 1;
                    } else {
                        int elen = node->len(i);
                        betapos -= elen - 1;
                        node = node->m_parent;
                    }

                    if (node->m_suffix == nullptr) {
                        betapos -= node->len(i);
                        node = node->m_parent;
                    }
                }

                node = node->m_suffix;
                if (node == m_root) betapos = j;

                bool done = false;
                startj = j + 1;

                while ((betapos <= i) && !done) {
                    char bchar = m_string[betapos];
                    unsigned char b = b2i(bchar);
                    SuffixNode *child = node->m_children[b];

                    if (!child) {
                        if (splitnode && betapos == splitnode->m_start) {
                            splitnode->m_parent->m_suffix = node;
                            splitnode = nullptr;
                        }

                        SuffixNode *newnode = new SuffixNode(betapos, len, j, node, nullptr);
                        node->m_children[b] = newnode;
                        lastleaf = newnode;
                        node = newnode;
                        nodewalk = 0;
                        done = true;
                        break;
                    } else {
                        int nodepos = child->m_start;
                        nodewalk = 0;

                        int mustmatch = i - 1 - betapos + 1;
                        int childlen = child->len(i);

                        if (mustmatch >= childlen) {
                            betapos += childlen;
                            nodepos += childlen;
                        } else if (mustmatch) {
                            betapos += mustmatch;
                            nodepos += mustmatch;
                            nodewalk += mustmatch;
                        }

                        while (nodepos <= child->m_end && betapos <= i) {
                            char nodebase = m_string[nodepos];
                            if (m_string[betapos] == nodebase) {
                                if (splitnode && betapos == splitnode->m_start) {
                                    splitnode->m_parent->m_suffix = node;
                                    splitnode = nullptr;
                                }
                                nodepos++; betapos++; nodewalk++;

                                if (betapos == i + 1) {
                                    if ((nodewalk == child->len(i)) && (child->m_end == len)) {
                                        // Leaf node implicit rule I
                                    } else {
                                        startj = j;
                                        j = i + 1; // skip rest of phase
                                    }
                                    done = true;
                                }
                            } else {
                                SuffixNode *split = new SuffixNode(child->m_start, nodepos - 1, 0, node, nullptr);
                                split->m_children[b2i(nodebase)] = child;
                                child->m_start = nodepos;
                                child->m_parent = split;

                                node->m_children[b] = split;
                                node = split;

                                if (splitnode && betapos == splitnode->m_start) {
                                    splitnode->m_parent->m_suffix = split;
                                    splitnode = nullptr;
                                }

                                SuffixNode *newnode = new SuffixNode(betapos, len, j, split, nullptr);
                                lastleaf = newnode;
                                split->m_children[b2i(m_string[betapos])] = newnode;
                                splitnode = newnode;
                                node = newnode;
                                nodewalk = 0;
                                done = true;
                                break;
                            }
                        }
                    }
                    if (!done) node = child;
                }
            }
        }
    }
};

inline void flatten_tree(SuffixNode *node,
                         std::vector<int> &n_start,
                         std::vector<int> &n_end,
                         std::vector<int> &n_suffix,
                         std::vector<int> &n_children) {
    if (!node) return;
    int id = node->id();
    n_start[id] = node->m_start;
    n_end[id] = node->m_end;
    n_suffix[id] = node->m_suffix ? node->m_suffix->id() : 1;
    for (int b = 0; b < 4; ++b) {
        if (node->m_children[b]) {
            n_children[id * 4 + b] = node->m_children[b]->id();
            flatten_tree(node->m_children[b], n_start, n_end, n_suffix, n_children);
        }
    }
    if (node->m_children[4]) {
        flatten_tree(node->m_children[4], n_start, n_end, n_suffix, n_children);
    }
}
