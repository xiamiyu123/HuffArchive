#pragma once

#include "StaticLinkedList.h"
#include <map>
#include <string>

namespace Structure {

class HuffmanTree {
public:
    HuffmanTree();
    ~HuffmanTree();

    // TODO: Implement tree building
    void build(const std::map<unsigned char, int>& frequencyMap);

    // TODO: Implement encoding generation
    std::map<unsigned char, std::string> generateCodes();

    // TODO: Implement decoding
    // void decode(...);
};

}
