#pragma once

#include "HuffmanNode.h"
#include "ArrayList.h"
#include "HashMap.h"
#include "PriorityQueue.h"
#include "String.h"
#include <functional>

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

    // [旧接口] 译码：将 01 字符串转换回原始字节流
    ArrayList<unsigned char> decode(const String& binaryString);

    // [新接口] 流式译码
    // readBit: 回调函数指针，返回下一个位 (0/1)，结束返回 -1
    // writeByte: 回调函数指针，输出解码出的一个字节
    // targetSize: 目标原始字节数 (用于解决 Padding 问题)
    void decode(std::function<int()> readBit, std::function<void(unsigned char)> writeByte, long long targetSize);

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
