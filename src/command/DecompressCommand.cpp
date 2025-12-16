#include "command/DecompressCommand.h"
#include "structure/HuffmanTree.h"
#include "io/FileHandler.h"
#include "io/BitStream.h"
#include "util/CryptoUtils.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

namespace Command {

DecompressCommand::DecompressCommand(Model::DataModel* model, const Structure::String& inputPath, const Structure::String& outputDir, const std::string& password)
    : m_model(model), m_inputPath(inputPath), m_outputDir(outputDir), m_password(password) {
}

DecompressCommand::~DecompressCommand() {
}

void DecompressCommand::execute() {
    if (!m_model) return;
    m_model->clear();

    std::filesystem::path inPath(reinterpret_cast<const char8_t*>(m_inputPath.c_str()));
    std::ifstream inFile(inPath, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open input file: " << m_inputPath.c_str() << std::endl;
        return;
    }

    // 1. 校验 Magic
    char magic[5] = {0};
    inFile.read(magic, 4);
    if (std::string(magic) != "HUFF") {
        std::cerr << "Invalid file format" << std::endl;
        return;
    }

    // 读取 Flag
    char flag = 0;
    inFile.read(&flag, 1);
    bool isEncrypted = (flag & 0x01);

    Util::CryptoUtils::StreamCipher* cipher = nullptr;

    if (isEncrypted) {
        char salt[8];
        inFile.read(salt, 8);
        
        char fileHash[16];
        inFile.read(fileHash, 16);

        if (m_password.empty()) {
            std::cerr << "Password required" << std::endl;
            return; // 或者抛出异常/回调通知
        }

        // 验证密码
        unsigned char computedHash[16];
        Util::CryptoUtils::hashPassword(m_password, reinterpret_cast<const unsigned char*>(salt), computedHash);
        
        if (memcmp(fileHash, computedHash, 16) != 0) {
            std::cerr << "Invalid password" << std::endl;
            return; // 或者抛出异常/回调通知
        }

        // 初始化 Cipher
        cipher = new Util::CryptoUtils::StreamCipher(m_password, reinterpret_cast<const unsigned char*>(salt));
    }

    auto readEncrypted = [&](char* buf, size_t size) {
        inFile.read(buf, size);
        if (cipher) {
            cipher->process(buf, size);
        }
    };

    // 2. 读取频率表
    int mapSize = 0;
    readEncrypted(reinterpret_cast<char*>(&mapSize), sizeof(int));
    
    Structure::HashMap<unsigned char, int> freqMap;
    for (int i = 0; i < mapSize; ++i) {
        unsigned char c;
        int f;
        readEncrypted(reinterpret_cast<char*>(&c), 1);
        readEncrypted(reinterpret_cast<char*>(&f), sizeof(int));
        freqMap.put(c, f);
    }

    // 3. 重建哈夫曼树
    Structure::HuffmanTree tree;
    tree.build(freqMap);

    // 4. 读取文件数量
    int fileCount = 0;
    readEncrypted(reinterpret_cast<char*>(&fileCount), sizeof(int));

    // 5. 读取目录并填充 DataModel
    for (int i = 0; i < fileCount; ++i) {
        int pathLen = 0;
        readEncrypted(reinterpret_cast<char*>(&pathLen), sizeof(int));
        
        char* pathBuf = new char[pathLen + 1];
        readEncrypted(pathBuf, pathLen);
        pathBuf[pathLen] = '\0';
        Structure::String relPath(pathBuf);
        delete[] pathBuf;
        
        long long origSize, compSize, offset;
        readEncrypted(reinterpret_cast<char*>(&origSize), sizeof(long long));
        readEncrypted(reinterpret_cast<char*>(&compSize), sizeof(long long));
        readEncrypted(reinterpret_cast<char*>(&offset), sizeof(long long));
        
        // 创建 FileRecord
        Model::FileRecord record("", Model::FileType::File);
        record.setRelativePath(relPath);
        record.setOriginalSize(origSize);
        record.setCompressedSize(compSize);
        record.setOffset(offset);
        record.setStatus(Model::FileStatus::Pending);
        
        m_model->addFile(record);
    }

    // 6. 解压所有文件
    // 确保输出目录存在
    std::filesystem::path outDir(reinterpret_cast<const char8_t*>(m_outputDir.c_str()));
    std::filesystem::create_directories(outDir);

    // 计算总大小用于进度条
    long long totalSize = 0;
    for (int i = 0; i < m_model->getFileCount(); ++i) {
        totalSize += m_model->getFile(i).getCompressedSize();
    }
    long long processedSize = 0;

    IO::BitStream bitStream;
    for (int i = 0; i < m_model->getFileCount(); ++i) {
        // 检查取消
        if (m_checkCancelCallback && m_checkCancelCallback()) {
            inFile.close();
            if (cipher) delete cipher;
            return;
        }

        Model::FileRecord& record = m_model->getFile(i);
        record.setStatus(Model::FileStatus::Processing);
        
        // 验证偏移量 (对于加密流，必须顺序读取)
        if (inFile.tellg() != record.getOffset()) {
             // 如果不匹配，说明逻辑有误或者文件损坏
             // 对于非加密文件，可以 seekg。对于加密文件，必须刚好在这里。
             if (!cipher) {
                 inFile.seekg(record.getOffset());
             } else {
                 // 如果有偏差，尝试跳过（虽然理论上不应该发生）
                 // 或者报错
                 // std::cerr << "Stream sync error" << std::endl;
             }
        }
        
        // 读取压缩数据
        long long size = record.getCompressedSize();
        
        if (size > 0) {
            Structure::ArrayList<unsigned char> buffer;
            char* tempBuf = new char[size];
            readEncrypted(tempBuf, size);
            
            for(long long k=0; k<size; ++k) {
                buffer.add(static_cast<unsigned char>(tempBuf[k]));
            }
            delete[] tempBuf;
            
            // 解码
            bitStream.loadBytes(buffer);
            
            auto readBitFunc = [&]() -> int {
                return bitStream.readBit();
            };

            // 构造完整输出路径
            std::filesystem::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
            std::filesystem::path outPath = outDir / relPath;
            
            std::filesystem::create_directories(outPath.parent_path());
            std::ofstream outFile(outPath, std::ios::binary);
            
            auto writeByteFunc = [&](unsigned char b) {
                outFile.put(static_cast<char>(b));
            };

            tree.decode(readBitFunc, writeByteFunc, record.getOriginalSize());
            outFile.close();
        } else {
            // 空文件
             std::filesystem::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
             std::filesystem::path outPath = outDir / relPath;
             std::filesystem::create_directories(outPath.parent_path());
             std::ofstream emptyFile(outPath, std::ios::binary);
             emptyFile.close();
        }
        
        record.setStatus(Model::FileStatus::Completed);

        // 更新进度
        processedSize += size;
        if (m_progressCallback && totalSize > 0) {
            m_progressCallback(static_cast<float>(processedSize) / totalSize);
        }
    }

    if (cipher) delete cipher;
    inFile.close();
}

}
