#include "command/CompressCommand.h"
#include "structure/HuffmanTree.h"
#include "io/FileHandler.h"
#include "io/BitStream.h"
#include <fstream>
#include <iostream>

namespace Command {

CompressCommand::CompressCommand(Model::DataModel* model, const Structure::String& outputPath)
    : m_model(model), m_outputPath(outputPath) {
}

CompressCommand::~CompressCommand() {
}

void CompressCommand::execute() {
    if (!m_model || m_model->getFileCount() == 0) return;

    // 1. 统计频率 (Pass 1)
    Structure::HashMap<unsigned char, int> freqMap;
    int fileCount = m_model->getFileCount();

    for (int i = 0; i < fileCount; ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        record.setStatus(Model::FileStatus::Processing);
        
        auto data = IO::FileHandler::readBinary(record.getFilePath());
        for (int j = 0; j < data.size(); ++j) {
            unsigned char c = data[j];
            if (freqMap.contains(c)) {
                freqMap[c]++;
            } else {
                freqMap.put(c, 1);
            }
        }
    }

    // 2. 构建哈夫曼树
    Structure::HuffmanTree tree;
    tree.build(freqMap);
    
    // 3. 打开输出文件
    std::ofstream outFile(m_outputPath.c_str(), std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << m_outputPath.c_str() << std::endl;
        return;
    }

    // 4. 写入文件头
    const char* magic = "HUFF";
    outFile.write(magic, 4);

    // 写入频率表
    int mapSize = freqMap.size();
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(int));
    
    for (const auto& pair : freqMap) {
        unsigned char c = pair.first;
        int f = pair.second;
        outFile.write(reinterpret_cast<const char*>(&c), 1);
        outFile.write(reinterpret_cast<const char*>(&f), sizeof(int));
    }

    // 写入文件数量
    outFile.write(reinterpret_cast<const char*>(&fileCount), sizeof(int));

    // 5. 记录目录起始位置，写入占位符
    long long dirStartPos = outFile.tellp();
    
    for (int i = 0; i < fileCount; ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        Structure::String path = record.getRelativePath();
        int pathLen = path.length();
        
        outFile.write(reinterpret_cast<const char*>(&pathLen), sizeof(int));
        outFile.write(path.c_str(), pathLen);
        
        long long zero = 0;
        outFile.write(reinterpret_cast<const char*>(&zero), sizeof(long long)); // Orig
        outFile.write(reinterpret_cast<const char*>(&zero), sizeof(long long)); // Comp
        outFile.write(reinterpret_cast<const char*>(&zero), sizeof(long long)); // Offset
    }

    // 6. 写入数据 (Pass 2)
    IO::BitStream bitStream;
    
    for (int i = 0; i < fileCount; ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        
        // 记录当前偏移量
        long long currentOffset = outFile.tellp();
        record.setOffset(currentOffset);
        
        // 读取并编码
        auto data = IO::FileHandler::readBinary(record.getFilePath());
        record.setOriginalSize(data.size());
        
        Structure::String encodedStr = tree.encode(data);
        
        // 写入 BitStream
        bitStream.clear();
        bitStream.writeBits(encodedStr);
        auto bytes = bitStream.getBytes();
        
        // 写入文件
        if (bytes.size() > 0) {
            // ArrayList 没有 data()，使用 &bytes[0]
            outFile.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
        }
        
        // 记录压缩后大小
        record.setCompressedSize(bytes.size());
        record.setStatus(Model::FileStatus::Completed);
    }

    // 7. 回填目录
    outFile.seekp(dirStartPos);
    for (int i = 0; i < fileCount; ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        Structure::String path = record.getRelativePath();
        int pathLen = path.length();
        
        outFile.write(reinterpret_cast<const char*>(&pathLen), sizeof(int));
        outFile.write(path.c_str(), pathLen);
        
        long long origSize = record.getOriginalSize();
        long long compSize = record.getCompressedSize();
        long long offset = record.getOffset();
        
        outFile.write(reinterpret_cast<const char*>(&origSize), sizeof(long long));
        outFile.write(reinterpret_cast<const char*>(&compSize), sizeof(long long));
        outFile.write(reinterpret_cast<const char*>(&offset), sizeof(long long));
    }

    outFile.close();
}

}
