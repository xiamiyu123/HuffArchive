#include "command/SelectiveDecompressCommand.h"
#include "model/DataModel.h"
#include "structure/HuffmanTree.h"
#include "io/BitStream.h"
#include "io/FileHandler.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace Command {

SelectiveDecompressCommand::SelectiveDecompressCommand(const Structure::String& inputPath,
                                                       const Structure::String& outputDir,
                                                       const Structure::ArrayList<Structure::String>& fileFilter)
    : m_inputPath(inputPath), m_outputDir(outputDir), m_fileFilter(fileFilter),
      m_extractedCount(0), m_skippedCount(0) {
}

SelectiveDecompressCommand::~SelectiveDecompressCommand() {
}

bool SelectiveDecompressCommand::execute() {
    try {
        // 1. 验证输入文件
        fs::path inputPath(m_inputPath.c_str());
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
        fs::path outputPath(m_outputDir.c_str());
        fs::create_directories(outputPath);

        // 3. 打开压缩文件并读取头信息
        std::ifstream inFile(m_inputPath.c_str(), std::ios::binary);
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

        // 5. 读取频率表以重建哈夫曼树
        int mapSize = 0;
        inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(int));
        
        Structure::HashMap<unsigned char, int> freqMap;
        for (int i = 0; i < mapSize; ++i) {
            unsigned char c;
            int f;
            inFile.read(reinterpret_cast<char*>(&c), 1);
            inFile.read(reinterpret_cast<char*>(&f), sizeof(int));
            freqMap.put(c, f);
        }

        // 重建哈夫曼树
        Structure::HuffmanTree tree;
        tree.build(freqMap);

        // 6. 读取文件数量和目录信息
        int fileCount = 0;
        inFile.read(reinterpret_cast<char*>(&fileCount), sizeof(int));

        // 读取所有文件的元数据
        Structure::ArrayList<Model::FileRecord> allFiles;
        for (int i = 0; i < fileCount; ++i) {
            int pathLen = 0;
            inFile.read(reinterpret_cast<char*>(&pathLen), sizeof(int));
            
            char* pathBuf = new char[pathLen + 1];
            inFile.read(pathBuf, pathLen);
            pathBuf[pathLen] = '\0';
            Structure::String relPath(pathBuf);
            delete[] pathBuf;
            
            long long origSize, compSize, offset;
            inFile.read(reinterpret_cast<char*>(&origSize), sizeof(long long));
            inFile.read(reinterpret_cast<char*>(&compSize), sizeof(long long));
            inFile.read(reinterpret_cast<char*>(&offset), sizeof(long long));
            
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

        for (int i = 0; i < allFiles.size(); ++i) {
            Model::FileRecord& record = allFiles[i];
            
            // 检查是否在过滤列表中
            if (!isFileInFilter(record.getRelativePath())) {
                m_skippedCount++;
                continue;
            }

            record.setStatus(Model::FileStatus::Processing);
            
            // 跳转到数据位置
            inFile.seekg(record.getOffset());
            
            // 读取压缩数据
            long long size = record.getCompressedSize();
            if (size > 0) {
                Structure::ArrayList<unsigned char> buffer;
                char* tempBuf = new char[size];
                inFile.read(tempBuf, size);
                
                for (long long k = 0; k < size; ++k) {
                    buffer.add(static_cast<unsigned char>(tempBuf[k]));
                }
                delete[] tempBuf;
                
                // 解码
                IO::BitStream bitStream;
                bitStream.loadBytes(buffer);
                
                // 构造输出路径
                std::filesystem::path outPath = std::filesystem::path(m_outputDir.c_str()) 
                                                / record.getRelativePath().c_str();
                std::filesystem::create_directories(outPath.parent_path());
                std::ofstream outFile(outPath.string().c_str(), std::ios::binary);
                
                // 执行解码
                auto readBitFunc = [&]() -> int {
                    return bitStream.readBit();
                };
                
                auto writeByteFunc = [&](unsigned char b) {
                    outFile.put(static_cast<char>(b));
                };
                
                tree.decode(readBitFunc, writeByteFunc, record.getOriginalSize());
                outFile.close();
                
                m_extractedCount++;
            } else {
                // 空文件
                std::filesystem::path outPath = std::filesystem::path(m_outputDir.c_str()) 
                                               / record.getRelativePath().c_str();
                std::filesystem::create_directories(outPath.parent_path());
                std::ofstream emptyFile(outPath.string().c_str());
                emptyFile.close();
                
                m_extractedCount++;
            }
            
            record.setStatus(Model::FileStatus::Completed);
        }

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
