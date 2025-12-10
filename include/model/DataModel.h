#pragma once

#include "structure/String.h"
#include "structure/HashMap.h"
#include "structure/ArrayList.h"
#include "structure/HuffmanTree.h"

namespace Model {

class DataModel {
public:
    DataModel();
    ~DataModel();

    // 设置输入文本并执行完整的哈夫曼编码流程
    void processText(const Structure::String& text);

    // 获取原始文本
    const Structure::String& getOriginalText() const;

    // 获取字符频率统计
    const Structure::HashMap<unsigned char, int>& getFrequencies() const;

    // 获取哈夫曼树
    const Structure::HuffmanTree& getTree() const;

    // 获取编码表
    const Structure::HashMap<unsigned char, Structure::String>& getCodes() const;

    // 获取编码后的二进制字符串
    const Structure::String& getEncodedString() const;

    // 获取解码后的文本（用于验证）
    Structure::String getDecodedString();

    // 计算压缩比 (原始大小 / 压缩后大小)
    double getCompressionRatio() const;

private:
    Structure::String m_originalText;
    Structure::HashMap<unsigned char, int> m_frequencies;
    Structure::HuffmanTree m_tree;
    Structure::HashMap<unsigned char, Structure::String> m_codes;
    Structure::String m_encodedString;
    
    void calculateFrequencies();
};

}
