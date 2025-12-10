#pragma once

#include "structure/String.h"
#include "structure/ArrayList.h"

namespace IO {

class BitStream {
public:
    BitStream();
    ~BitStream();

    // 写入一个位（0 或 1）
    void writeBit(int bit);

    // 写入多个位（从字符串 "0101..."）
    void writeBits(const Structure::String& bits);

    // 写入一个字节（8位）
    void writeByte(unsigned char byte);

    // 获取当前缓冲区的所有字节数据
    // 如果最后不足8位，会自动补0
    Structure::ArrayList<unsigned char> getBytes();

    // 从字节数组加载数据用于读取
    void loadBytes(const Structure::ArrayList<unsigned char>& data);

    // 读取一个位（返回 0 或 1，如果读完返回 -1）
    int readBit();

    // 清空缓冲区
    void clear();

private:
    Structure::ArrayList<unsigned char> m_buffer; // 存储完整的字节
    unsigned char m_currentByte;       // 当前正在构建/读取的字节
    int m_bitCount;                    // 当前字节已写入/读取的位数
    
    // 读取模式下的状态
    int m_readByteIndex;               // 当前读取到的字节索引
    int m_readBitIndex;                // 当前字节读取到的位索引
};

}
