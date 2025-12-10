#include "model/DataModel.h"
#include <iostream>

namespace Model {

DataModel::DataModel() {
}

DataModel::~DataModel() {
}

void DataModel::processText(const Structure::String& text) {
    m_originalText = text;
    m_encodedString = "";
    m_codes.clear();
    
    if (text.empty()) {
        return;
    }

    // 1. 统计频率
    calculateFrequencies();
    
    // 2. 构建哈夫曼树
    m_tree.build(m_frequencies);
    
    // 3. 生成编码表
    m_codes = m_tree.generateCodes();
    
    // 4. 编码
    m_encodedString = m_tree.encode(reinterpret_cast<const unsigned char*>(m_originalText.c_str()), m_originalText.length());
}

void DataModel::calculateFrequencies() {
    m_frequencies.clear();
    const char* data = m_originalText.c_str();
    int len = m_originalText.length();
    
    for (int i = 0; i < len; ++i) {
        unsigned char c = static_cast<unsigned char>(data[i]);
        m_frequencies[c]++;
    }
}

const Structure::String& DataModel::getOriginalText() const {
    return m_originalText;
}

const Structure::HashMap<unsigned char, int>& DataModel::getFrequencies() const {
    return m_frequencies;
}

const Structure::HuffmanTree& DataModel::getTree() const {
    return m_tree;
}

const Structure::HashMap<unsigned char, Structure::String>& DataModel::getCodes() const {
    return m_codes;
}

const Structure::String& DataModel::getEncodedString() const {
    return m_encodedString;
}

Structure::String DataModel::getDecodedString() {
    if (m_encodedString.empty()) return "";

    auto decodedBytes = m_tree.decode(m_encodedString);
    
    int len = decodedBytes.size();
    if (len == 0) return "";

    char* buffer = new char[len + 1];
    for(int i=0; i<len; ++i) {
        buffer[i] = static_cast<char>(decodedBytes[i]);
    }
    buffer[len] = '\0';
    
    Structure::String result(buffer);
    delete[] buffer;
    return result;
}

double DataModel::getCompressionRatio() const {
    if (m_originalText.length() == 0) return 0.0;
    if (m_encodedString.length() == 0) return 0.0;

    // 原始大小：每个字符 8 位
    long long originalBits = (long long)m_originalText.length() * 8;
    // 压缩后大小：编码字符串的长度（每个字符代表 1 位）
    long long compressedBits = m_encodedString.length();
    
    return (double)originalBits / (double)compressedBits;
}

}
