#include "command/DecompressCommand.h"
#include "structure/HuffmanTree.h"
#include "io/FileHandler.h"
#include "io/BitStream.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace Command {

DecompressCommand::DecompressCommand(Model::DataModel* model, const Structure::String& inputPath, const Structure::String& outputDir)
    : m_model(model), m_inputPath(inputPath), m_outputDir(outputDir) {
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

    // 2. 读取频率表
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

    // 3. 重建哈夫曼树
    Structure::HuffmanTree tree;
    tree.build(freqMap);

    // 4. 读取文件数量
    int fileCount = 0;
    inFile.read(reinterpret_cast<char*>(&fileCount), sizeof(int));

    // 5. 读取目录并填充 DataModel
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
        
        // 创建 FileRecord
        // 注意：这里我们没有源文件的绝对路径，只有相对路径
        // 我们构造一个 FileRecord，主要用于解压
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
            return;
        }

        Model::FileRecord& record = m_model->getFile(i);
        record.setStatus(Model::FileStatus::Processing);
        
        // 跳转到数据位置
        inFile.seekg(record.getOffset());
        
        // 读取压缩数据
        long long size = record.getCompressedSize();
        
        if (size > 0) {
            Structure::ArrayList<unsigned char> buffer;
            char* tempBuf = new char[size];
            inFile.read(tempBuf, size);
            
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

    inFile.close();
}

}
