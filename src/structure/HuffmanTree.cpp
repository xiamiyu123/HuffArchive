#include "structure/HuffmanTree.h"
#include <algorithm>
#include <limits>
#include <iostream>

namespace Structure {

HuffmanTree::HuffmanTree() : m_root(-1) {
}

HuffmanTree::~HuffmanTree() {
}

void HuffmanTree::build(const HashMap<unsigned char, int>& frequencyMap) {
    m_nodes.clear();
    m_root = -1;

    if (frequencyMap.empty()) {
        return;
    }

    int n = frequencyMap.size();
    int m = 2 * n - 1; // 总节点数
    m_nodes.resize(m);

    // 1. 初始化叶子节点
    int i = 0;
    for (const auto& pair : frequencyMap) {
        m_nodes[i].data = pair.first;
        m_nodes[i].weight = pair.second;
        m_nodes[i].parent = -1;
        m_nodes[i].lchild = -1;
        m_nodes[i].rchild = -1;
        i++;
    }

    // 初始化非叶子节点（其实 resize 默认构造已经做了，但为了保险喵）
    for (; i < m; ++i) {
        m_nodes[i].parent = -1;
        m_nodes[i].lchild = -1;
        m_nodes[i].rchild = -1;
        m_nodes[i].weight = 0;
    }

    // 2. 构建哈夫曼树
    // 从 n 开始，依次创建 n-1 个新节点
    for (i = n; i < m; ++i) {
        int s1 = -1, s2 = -1;
        // 在 0 到 i-1 范围内选择两个 parent 为 -1 且 weight 最小的节点
        select(i, s1, s2);

        // 新节点 i 是 s1 和 s2 的父节点
        m_nodes[s1].parent = i;
        m_nodes[s2].parent = i;

        m_nodes[i].lchild = s1;
        m_nodes[i].rchild = s2;
        m_nodes[i].weight = m_nodes[s1].weight + m_nodes[s2].weight;
    }

    m_root = m - 1; // 最后一个生成的节点即为根节点
}

void HuffmanTree::select(int endIndex, int& s1, int& s2) {
    int min1 = std::numeric_limits<int>::max();
    int min2 = std::numeric_limits<int>::max();
    s1 = -1;
    s2 = -1;

    for (int i = 0; i < endIndex; ++i) {
        if (m_nodes[i].parent != -1) {
            continue; // 已经有父节点了，跳过
        }

        if (m_nodes[i].weight < min1) {
            // 更新最小值，原最小值变为次小值
            min2 = min1;
            s2 = s1;
            min1 = m_nodes[i].weight;
            s1 = i;
        } else if (m_nodes[i].weight < min2) {
            // 更新次小值
            min2 = m_nodes[i].weight;
            s2 = i;
        }
    }
}

//Ciallo～ (∠・ω< )⌒★

HashMap<unsigned char, std::string> HuffmanTree::generateCodes() {
    HashMap<unsigned char, std::string> codes;
    if (m_root == -1) return codes;

    int n = (m_nodes.size() + 1) / 2; // 叶子节点数量

    // 特殊情况：只有一个字符
    if (n == 1) {
        codes[m_nodes[0].data] = "0";
        return codes;
    }

    // 遍历所有叶子节点
    for (int i = 0; i < n; ++i) {
        std::string code = "";
        int current = i;
        int parent = m_nodes[current].parent;

        // 从叶子向上回溯到根
        while (parent != -1) {
            if (m_nodes[parent].lchild == current) {
                code += '0'; // 左孩子为 0
            } else {
                code += '1'; // 右孩子为 1
            }
            current = parent;
            parent = m_nodes[current].parent;
        }

        // 回溯得到的是反向编码，需要翻转
        std::reverse(code.begin(), code.end());
        codes[m_nodes[i].data] = code;
    }

    return codes;
}

ArrayList<unsigned char> HuffmanTree::decode(const std::string& binaryString) {
    ArrayList<unsigned char> result;
    if (m_root == -1) return result;

    int current = m_root;
    for (char bit : binaryString) {
        if (bit == '0') {
            current = m_nodes[current].lchild;
        } else if (bit == '1') {
            current = m_nodes[current].rchild;
        }

        // 如果到达叶子节点
        if (m_nodes[current].lchild == -1 && m_nodes[current].rchild == -1) {
            result.add(m_nodes[current].data);
            current = m_root; // 重置回根节点，准备解码下一个字符
        }
    }

    return result;
}

int HuffmanTree::getRootIndex() const {
    return m_root;
}

const ArrayList<HuffmanNode<unsigned char>>& HuffmanTree::getNodes() const {
    return m_nodes;
}

std::string HuffmanTree::encode(const ArrayList<unsigned char>& data) {
    return encode(data.begin(), data.size());
}

std::string HuffmanTree::encode(const unsigned char* data, int length) {
    if (m_root == -1 || length == 0) return "";

    // 先生成编码表
    auto codes = generateCodes();
    
    std::string result;
    for (int i = 0; i < length; ++i) {
        auto it = codes.find(data[i]);
        if (it != codes.end()) {
            result += it->second;
        }
        // 如果字符不在编码表中，跳过（或者可以抛出异常）
    }
    
    return result;
}

}
