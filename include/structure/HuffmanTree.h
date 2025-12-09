#pragma once

#include "HuffmanNode.h"
#include "ArrayList.h"
#include "HashMap.h"
#include <string>

namespace Structure {

class HuffmanTree {
public:
    HuffmanTree();
    ~HuffmanTree();

    // 构建哈夫曼树
    void build(const HashMap<unsigned char, int>& frequencyMap);

    // 生成哈夫曼编码表
    HashMap<unsigned char, std::string> generateCodes();

    // 编码：将字节数组编码为 01 字符串
    std::string encode(const ArrayList<unsigned char>& data);
    std::string encode(const unsigned char* data, int length);

    // 译码：将 01 字符串转换回原始字节流
    // 返回解码后的字节数组
    ArrayList<unsigned char> decode(const std::string& binaryString);

    // 获取根节点索引
    int getRootIndex() const;
    
    // 获取节点列表
    const ArrayList<HuffmanNode<unsigned char>>& getNodes() const;

private:
    // 链表存储结构：使用 ArrayList 存储节点
    ArrayList<HuffmanNode<unsigned char>> m_nodes;
    int m_root; // 根节点索引

    // 辅助函数：在 m_nodes[0...endIndex] 中寻找两个权值最小且无父节点的节点
    // s1 为最小，s2 为次小
    // 结果通过引用参数返回
    void select(int endIndex, int& s1, int& s2);
};

}
