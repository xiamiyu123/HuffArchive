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

    // 定义比较器：比较两个节点索引对应的权值
    struct NodeComparator {
        const ArrayList<HuffmanNode<unsigned char>>& nodes;
        NodeComparator(const ArrayList<HuffmanNode<unsigned char>>& n) : nodes(n) {}
        
        // 返回 true 如果 a 的优先级高于 b（即 a 的权值小于 b）
        bool operator()(int a, int b) const {
            if (nodes[a].weight != nodes[b].weight) {
                return nodes[a].weight < nodes[b].weight;
            }
            return a < b; // 权值相同时，索引小的优先，保证稳定性
        }
    };

    // 创建优先队列
    NodeComparator comp(m_nodes);
    PriorityQueue<int, NodeComparator> pq(comp);

    // 将所有叶子节点的索引加入优先队列
    for (int j = 0; j < n; ++j) {
        pq.push(j);
    }

    // 2. 构建哈夫曼树
    
    // 特殊情况：只有一个字符 (n=1)
    // 为了保证树结构的统一性，我们创建一个虚拟根节点
    if (n == 1) {
        m_nodes.resize(2);
        // Node 0: 已经在上面初始化好了 (data, weight)
        
        // Node 1: 虚拟根节点
        m_nodes[1].lchild = 0;  // 左孩子指向唯一的字符
        m_nodes[1].rchild = -1; // 右孩子为空
        m_nodes[1].weight = m_nodes[0].weight;
        m_nodes[1].parent = -1;
        
        m_nodes[0].parent = 1; // 字符节点的父节点是根
        
        m_root = 1;
        return;
    }

    // 循环直到队列中只剩下一个节点（根节点）
    // 注意：我们需要生成 n-1 个新节点，新节点的索引从 n 开始
    int nextNodeIndex = n;
    while (pq.size() > 1) {
        // 取出两个权值最小的节点
        int s1 = pq.top();
        pq.pop();
        int s2 = pq.top();
        pq.pop();

        // 新节点 nextNodeIndex 是 s1 和 s2 的父节点
        m_nodes[s1].parent = nextNodeIndex;
        m_nodes[s2].parent = nextNodeIndex;

        m_nodes[nextNodeIndex].lchild = s1;
        m_nodes[nextNodeIndex].rchild = s2;
        m_nodes[nextNodeIndex].weight = m_nodes[s1].weight + m_nodes[s2].weight;

        // 将新生成的父节点加入队列
        pq.push(nextNodeIndex);
        
        nextNodeIndex++;
    }

    m_root = m - 1; // 最后一个生成的节点即为根节点
}

// select 函数已移除，使用优先队列替代

//Ciallo～ (∠・ω< )⌒★

HashMap<unsigned char, String> HuffmanTree::generateCodes() {
    HashMap<unsigned char, String> codes;
    if (m_root == -1) return codes;

    int n = (m_nodes.size() + 1) / 2; // 叶子节点数量
    
    // 注意：n=1 的特殊情况现在由 build() 中的虚拟根节点处理了
    // 遍历逻辑通用

    // 遍历所有叶子节点
    for (int i = 0; i < n; ++i) {
        String code = "";
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

ArrayList<unsigned char> HuffmanTree::decode(const String& binaryString) {
    ArrayList<unsigned char> result;
    if (m_root == -1) return result;

    // 使用新的流式接口实现旧接口
    // 构造一个简单的 bit provider
    int index = 0;
    int len = binaryString.length();
    auto readBit = [&]() -> int {
        if (index >= len) return -1;
        char c = binaryString[index++];
        return (c == '1') ? 1 : 0;
    };

    // 构造一个简单的 byte writer
    auto writeByte = [&](unsigned char b) {
        result.add(b);
    };

    // targetSize 设置为最大值，因为旧接口不处理 padding
    decode(readBit, writeByte, std::numeric_limits<long long>::max());
    
    return result;
}

void HuffmanTree::decode(std::function<int()> readBit, std::function<void(unsigned char)> writeByte, long long targetSize) {
    if (m_root == -1) return;

    int current = m_root;
    long long decodedCount = 0;

    while (decodedCount < targetSize) {
        // 每次循环读取一个位
        int bit = readBit();
        if (bit == -1) break; // 流结束

        if (bit == 0) {
            current = m_nodes[current].lchild;
        } else {
            current = m_nodes[current].rchild;
        }

        // 检查是否跑飞了 (例如 n=1 时遇到 '1')
        if (current == -1) {
            // 错误恢复：重置到根节点，丢弃当前位
            current = m_root;
            continue;
        }

        // 如果到达叶子节点
        if (m_nodes[current].lchild == -1 && m_nodes[current].rchild == -1) {
            writeByte(m_nodes[current].data);
            decodedCount++;
            current = m_root; // 重置回根节点
        }
    }
}

int HuffmanTree::getRootIndex() const {
    return m_root;
}

const ArrayList<HuffmanNode<unsigned char>>& HuffmanTree::getNodes() const {
    return m_nodes;
}

String HuffmanTree::encode(const ArrayList<unsigned char>& data) {
    return encode(data.begin(), data.size());
}

String HuffmanTree::encode(const unsigned char* data, int length) {
    if (m_root == -1 || length == 0) return "";

    // 先生成编码表
    auto codes = generateCodes();
    
    String result;
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
