#include "util/CryptoUtils.h"
#include <chrono>
#include <functional>
#include <sstream>

namespace Util {

void CryptoUtils::generateSalt(unsigned char* salt) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < SALT_SIZE; ++i) {
        salt[i] = static_cast<unsigned char>(dis(gen));
    }
}

void CryptoUtils::hashPassword(const std::string& password, const unsigned char* salt, unsigned char* hash) {
    // 简单的哈希函数：FNV-1a 混合盐值
    uint64_t h = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;

    auto update = [&](unsigned char byte) {
        h ^= byte;
        h *= prime;
    };

    for (char c : password) {
        update(static_cast<unsigned char>(c));
    }
    for (int i = 0; i < SALT_SIZE; ++i) {
        update(salt[i]);
    }

    // 将 64 位哈希折叠成 16 字节（重复）
    // 为了使其稍微更健壮，我们将运行几轮
    for (int round = 0; round < 100; ++round) {
        update(static_cast<unsigned char>(h & 0xFF));
    }

    // 填充 16 字节的哈希缓冲区
    std::mt19937 gen(static_cast<unsigned int>(h));
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < HASH_SIZE; ++i) {
        hash[i] = static_cast<unsigned char>(dis(gen));
    }
}

CryptoUtils::StreamCipher::StreamCipher(const std::string& password, const unsigned char* salt) {
    // 使用密码 + 盐值的哈希值作为 PRNG 的种子
    unsigned char hash[HASH_SIZE];
    CryptoUtils::hashPassword(password, salt, hash);
    
    // 从哈希值创建种子序列
    std::vector<unsigned int> seedData;
    for (int i = 0; i < HASH_SIZE; i += 4) {
        unsigned int val = 0;
        val |= hash[i];
        val |= (hash[i+1] << 8);
        val |= (hash[i+2] << 16);
        val |= (hash[i+3] << 24);
        seedData.push_back(val);
    }
    std::seed_seq seq(seedData.begin(), seedData.end());
    m_engine.seed(seq);
}

void CryptoUtils::StreamCipher::process(char* buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] ^= static_cast<char>(m_engine());
    }
}

unsigned char CryptoUtils::StreamCipher::processByte(unsigned char b) {
    return b ^ static_cast<unsigned char>(m_engine());
}

std::string CryptoUtils::StreamCipher::saveState() const {
    std::stringstream ss;
    ss << m_engine;
    return ss.str();
}

void CryptoUtils::StreamCipher::restoreState(const std::string& state) {
    std::stringstream ss(state);
    ss >> m_engine;
}

}
