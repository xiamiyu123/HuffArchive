#pragma once

#include "HuffmanNode.h"
#include "ArrayList.h"
#include "HashMap.h"
#include "PriorityQueue.h"
#include "String.h"

namespace Structure {

class HuffmanTree {
public:
    HuffmanTree();
    ~HuffmanTree();

    // 构建哈夫曼树
    void build(const HashMap<unsigned char, int>& frequencyMap);

    // 生成哈夫曼编码表
    HashMap<unsigned char, String> generateCodes();

    // 编码：将字节数组编码为 01 字符串
    String encode(const ArrayList<unsigned char>& data);
    String encode(const unsigned char* data, int length);

    // 译码：将 01 字符串转换回原始字节流
    // 返回解码后的字节数组
    ArrayList<unsigned char> decode(const String& binaryString);

    // 获取根节点索引
    int getRootIndex() const;
    
    // 获取节点列表
    const ArrayList<HuffmanNode<unsigned char>>& getNodes() const;

private:
    // 链表存储结构：使用 ArrayList 存储节点
    ArrayList<HuffmanNode<unsigned char>> m_nodes;
    int m_root; // 根节点索引
};

}
