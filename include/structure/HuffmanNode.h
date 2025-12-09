#pragma once

namespace Structure {

template <typename T>
struct HuffmanNode {
    // TODO: Define node data (weight, parent, children)
    T data;
    int weight;
    int parent;
    int lchild;
    int rchild;

    HuffmanNode() : weight(0), parent(-1), lchild(-1), rchild(-1) {}
};

}
