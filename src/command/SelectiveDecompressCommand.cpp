#include "command/SelectiveDecompressCommand.h"
#include "model/DataModel.h"
#include "structure/HuffmanTree.h"
#include "io/BitStream.h"
#include "io/FileHandler.h"
#include "util/CryptoUtils.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace Command {

SelectiveDecompressCommand::SelectiveDecompressCommand(const Structure::String& inputPath,
                                                       const Structure::String& outputDir,
                                                       const Structure::ArrayList<Structure::String>& fileFilter,
                                                       const std::string& password)
    : m_inputPath(inputPath), m_outputDir(outputDir), m_fileFilter(fileFilter), m_password(password),
      m_extractedCount(0), m_skippedCount(0) {
}

SelectiveDecompressCommand::~SelectiveDecompressCommand() {
}

bool SelectiveDecompressCommand::execute() {
    try {
        // 1. 验证输入文件
        fs::path inputPath(reinterpret_cast<const char8_t*>(m_inputPath.c_str()));
        if (!fs::exists(inputPath)) {
            m_errorMessage = Structure::String("Input archive does not exist: ");
            m_errorMessage = m_errorMessage + m_inputPath;
            return false;
        }

        if (!fs::is_regular_file(inputPath)) {
            m_errorMessage = Structure::String("Input path is not a file");
            return false;
        }

        // 2. 创建输出目录
        fs::path outputPath(reinterpret_cast<const char8_t*>(m_outputDir.c_str()));
        fs::create_directories(outputPath);

        // 3. 打开压缩文件并读取头信息
        std::ifstream inFile(inputPath, std::ios::binary);
        if (!inFile) {
            m_errorMessage = Structure::String("Failed to open input file: ");
            m_errorMessage = m_errorMessage + m_inputPath;
            return false;
        }

        // 4. 校验 Magic
        char magic[5] = {0};
        inFile.read(magic, 4);
        if (std::string(magic) != "HUFF") {
            m_errorMessage = Structure::String("Invalid file format");
            return false;
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
                m_errorMessage = Structure::String("Password required");
                return false;
            }

            // 验证密码
            unsigned char computedHash[16];
            Util::CryptoUtils::hashPassword(m_password, reinterpret_cast<const unsigned char*>(salt), computedHash);
            
            if (memcmp(fileHash, computedHash, 16) != 0) {
                m_errorMessage = Structure::String("Invalid password");
                return false;
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

        // 5. 读取频率表以重建哈夫曼树
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

        // 重建哈夫曼树
        Structure::HuffmanTree tree;
        tree.build(freqMap);

        // 6. 读取文件数量和目录信息
        int fileCount = 0;
        readEncrypted(reinterpret_cast<char*>(&fileCount), sizeof(int));

        // 读取所有文件的元数据
        Structure::ArrayList<Model::FileRecord> allFiles;
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
            
            Model::FileRecord record("", Model::FileType::File);
            record.setRelativePath(relPath);
            record.setOriginalSize(origSize);
            record.setCompressedSize(compSize);
            record.setOffset(offset);
            record.setStatus(Model::FileStatus::Pending);
            
            allFiles.add(record);
        }

        // 7. 按过滤条件解压文件
        m_extractedCount = 0;
        m_skippedCount = 0;

        // 计算总大小用于进度条
        long long totalSize = 0;
        for (int i = 0; i < allFiles.size(); ++i) {
            if (isFileInFilter(allFiles[i].getRelativePath())) {
                totalSize += allFiles[i].getCompressedSize();
            }
        }
        long long processedSize = 0;

        for (int i = 0; i < allFiles.size(); ++i) {
            // 检查取消
            if (m_checkCancelCallback && m_checkCancelCallback()) {
                inFile.close();
                if (cipher) delete cipher;
                return false;
            }

            Model::FileRecord& record = allFiles[i];
            bool isSelected = isFileInFilter(record.getRelativePath());
            
            // 如果没有加密，且不需要解压该文件，则直接跳过
            if (!cipher && !isSelected) {
                m_skippedCount++;
                continue;
            }

            record.setStatus(Model::FileStatus::Processing);
            
            // 跳转到数据位置 (仅非加密模式)
            if (!cipher) {
                inFile.seekg(record.getOffset());
            }
            
            // 读取压缩数据
            long long size = record.getCompressedSize();
            if (size > 0) {
                Structure::ArrayList<unsigned char> buffer;
                char* tempBuf = new char[size];
                readEncrypted(tempBuf, size); // 如果加密，这里会解密并推进状态
                
                if (isSelected) {
                    for (long long k = 0; k < size; ++k) {
                        buffer.add(static_cast<unsigned char>(tempBuf[k]));
                    }
                    
                    // 解码
                    IO::BitStream bitStream;
                    bitStream.loadBytes(buffer);
                    
                    auto readBitFunc = [&]() -> int {
                        return bitStream.readBit();
                    };

                    // 构造完整输出路径
                    fs::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
                    fs::path outPath = outputPath / relPath;
                    
                    fs::create_directories(outPath.parent_path());
                    std::ofstream outFile(outPath, std::ios::binary);
                    
                    auto writeByteFunc = [&](unsigned char b) {
                        outFile.put(static_cast<char>(b));
                    };

                    tree.decode(readBitFunc, writeByteFunc, record.getOriginalSize());
                    outFile.close();
                    
                    m_extractedCount++;
                    
                    // 更新进度
                    processedSize += size;
                    if (m_progressCallback && totalSize > 0) {
                        m_progressCallback(static_cast<float>(processedSize) / totalSize);
                    }
                } else {
                    m_skippedCount++;
                }
                delete[] tempBuf;
            } else {
                // 空文件
                if (isSelected) {
                    fs::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
                    fs::path outPath = outputPath / relPath;
                    fs::create_directories(outPath.parent_path());
                    std::ofstream emptyFile(outPath, std::ios::binary);
                    emptyFile.close();
                    m_extractedCount++;
                } else {
                    m_skippedCount++;
                }
            }
            
            record.setStatus(Model::FileStatus::Completed);
        }

        if (cipher) delete cipher;
        inFile.close();
        return true;
    } catch (const std::exception& e) {
        m_errorMessage = Structure::String(e.what());
        return false;
    }
}

Structure::String SelectiveDecompressCommand::getErrorMessage() const {
    return m_errorMessage;
}

int SelectiveDecompressCommand::getExtractedCount() const {
    return m_extractedCount;
}

int SelectiveDecompressCommand::getSkippedCount() const {
    return m_skippedCount;
}

bool SelectiveDecompressCommand::isFileInFilter(const Structure::String& filePath) const {
    // 如果过滤列表为空，则解压所有文件
    if (m_fileFilter.size() == 0) {
        return true;
    }

    // 检查文件是否在过滤列表中
    for (int i = 0; i < m_fileFilter.size(); ++i) {
        if (m_fileFilter[i] == filePath) {
            return true;
        }
    }
    return false;
}

}
