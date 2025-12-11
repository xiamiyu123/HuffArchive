#include "io/BitStream.h"

namespace IO {

BitStream::BitStream() 
    : m_currentByte(0), m_bitCount(0), m_readByteIndex(0), m_readBitIndex(0) {
}

BitStream::~BitStream() {
}

void BitStream::writeBit(int bit) {
    // 确保 bit 只有 0 或 1
    bit = bit ? 1 : 0;

    // 将 bit 写入当前字节的对应位置
    // 我们从高位到低位写入 (7 -> 0)
    if (bit) {
        m_currentByte |= (1 << (7 - m_bitCount));
    }

    m_bitCount++;

    // 如果当前字节写满了（8位），存入缓冲区并重置
    if (m_bitCount == 8) {
        m_buffer.add(m_currentByte);
        m_currentByte = 0;
        m_bitCount = 0;
    }
}

void BitStream::writeBits(const Structure::String& bits) {
    for (char c : bits) {
        if (c == '0') {
            writeBit(0);
        } else if (c == '1') {
            writeBit(1);
        }
    }
}

void BitStream::writeByte(unsigned char byte) {
    // 直接写入8个位
    for (int i = 7; i >= 0; --i) {
        writeBit((byte >> i) & 1);
    }
}

Structure::ArrayList<unsigned char> BitStream::getBytes() {
    Structure::ArrayList<unsigned char> result = m_buffer;
    
    // 如果还有未写满的字节，也需要加入（后面补0）
    if (m_bitCount > 0) {
        result.add(m_currentByte);
    }
    
    return result;
}

void BitStream::loadBytes(const Structure::ArrayList<unsigned char>& data) {
    m_buffer = data;
    m_readByteIndex = 0;
    m_readBitIndex = 0;
    m_currentByte = 0; // 读取模式下不使用这个作为缓存，直接读 m_buffer
    m_bitCount = 0;    // 也不用这个
}


int BitStream::readBit() {
    if (m_readByteIndex >= m_buffer.size()) {
        return -1; // 读完了
    }

    unsigned char byte = m_buffer[m_readByteIndex];
    int bit = (byte >> (7 - m_readBitIndex)) & 1;

    m_readBitIndex++;
    if (m_readBitIndex == 8) {
        m_readBitIndex = 0;
        m_readByteIndex++;
    }

    return bit;
}

void BitStream::clear() {
    m_buffer.clear();
    m_currentByte = 0;
    m_bitCount = 0;
    m_readByteIndex = 0;
    m_readBitIndex = 0;
}

}
