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

    std::ifstream inFile(m_inputPath.c_str(), std::ios::binary);
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
    std::filesystem::create_directories(m_outputDir.c_str());

    IO::BitStream bitStream;
    for (int i = 0; i < m_model->getFileCount(); ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        record.setStatus(Model::FileStatus::Processing);
        
        // 跳转到数据位置
        inFile.seekg(record.getOffset());
        
        // 读取压缩数据
        long long size = record.getCompressedSize();
        if (size > 0) {
            Structure::ArrayList<unsigned char> buffer;
            // 预分配 buffer? ArrayList 可能没有 resize/reserve 公开接口，只能循环 add
            // 或者一次性读取到 char* 然后构造 ArrayList
            char* tempBuf = new char[size];
            inFile.read(tempBuf, size);
            
            // 将 char* 转为 ArrayList<unsigned char>
            // 这是一个性能瓶颈，但为了兼容现有接口
            for(long long k=0; k<size; ++k) {
                buffer.add(static_cast<unsigned char>(tempBuf[k]));
            }
            delete[] tempBuf;
            
            // 解码
            bitStream.loadBytes(buffer);
            // BitStream 还原的是 01 串，我们需要 HuffmanTree::decode
            // Wait, HuffmanTree::decode takes String (0101...)
            // BitStream::readBit() gives bits.
            // We need to reconstruct the "0101..." string from BitStream?
            // NO! HuffmanTree::decode takes "0101..." string.
            // This is inefficient. Ideally HuffmanTree should decode from BitStream directly.
            // But based on existing HuffmanTree.h: `ArrayList<unsigned char> decode(const String& binaryString);`
            // So I must convert BitStream back to String "0101...".
            
            Structure::String binaryString = "";
            // 我们不知道确切的 bit 数，但我们知道原始字节数吗？
            // 不，我们只知道压缩后的字节数。
            // 解码时，HuffmanTree 会根据树结构走，直到解出所有字符？
            // 问题：BitStream 可能有 padding (补零)。
            // 如果我们把 padding 也解进去，可能会多出乱码。
            // 但是 HuffmanTree::decode 是根据 01 串走的。
            // 更好的做法是：decode 应该解码出 `originalSize` 个字节后停止。
            // 但目前的 HuffmanTree::decode 没有这个参数。
            // 让我们先生成完整的 01 串。
            
            // 这种实现方式内存消耗巨大，但符合当前接口定义。
            // 优化：修改 HuffmanTree::decode 支持流式或最大字符数。
            // 现阶段：生成 String。
            
            // BitStream 没有 "getAllBitsAsString" 方法。
            // 只能一个个 readBit。
            // 多少个 bit? size * 8。
            long long totalBits = size * 8;
            // 实际上有效 bit 数可能少于 totalBits (padding)。
            // 但我们不知道 padding 是多少。
            // 幸好，只要解出的字符数达到 originalSize，我们就可以停止。
            // 所以我们需要修改 HuffmanTree::decode 或者在这里手动解码。
            
            // 让我们在这里手动解码，利用 tree 的 public 接口
            // getNodes() 和 getRootIndex() 是 public 的。
            
            const auto& nodes = tree.getNodes();
            int root = tree.getRootIndex();
            int current = root;
            Structure::ArrayList<unsigned char> decodedData;
            
            long long decodedCount = 0;
            long long targetSize = record.getOriginalSize();
            
            // 重新加载 bitStream
            bitStream.loadBytes(buffer);
            
            while (decodedCount < targetSize) {
                int bit = bitStream.readBit();
                if (bit == -1) break; // Should not happen if logic is correct
                
                if (bit == 0) {
                    current = nodes[current].lchild;
                } else {
                    current = nodes[current].rchild;
                }
                
                if (nodes[current].lchild == -1 && nodes[current].rchild == -1) {
                    decodedData.add(nodes[current].data);
                    decodedCount++;
                    current = root;
                }
            }
            
            // 写入文件
            // 构造完整输出路径
            std::filesystem::path outPath = std::filesystem::path(m_outputDir.c_str()) / record.getRelativePath().c_str();
            // 确保父目录存在
            std::filesystem::create_directories(outPath.parent_path());
            
            IO::FileHandler::writeBinary(outPath.string().c_str(), decodedData);
        } else {
            // 空文件
             std::filesystem::path outPath = std::filesystem::path(m_outputDir.c_str()) / record.getRelativePath().c_str();
             std::filesystem::create_directories(outPath.parent_path());
             std::ofstream emptyFile(outPath.string().c_str());
             emptyFile.close();
        }
        
        record.setStatus(Model::FileStatus::Completed);
    }

    inFile.close();
}

}
