#include <QtTest>
#include <iostream>
#include "command/CompressCommand.h"
#include "command/DecompressCommand.h"
#include "model/DataModel.h"
#include "model/FileRecord.h"
#include "io/FileHandler.h"
#include "structure/ArrayList.h"
#include <filesystem>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

class TestCommand : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // 创建临时目录
        m_testDir = fs::temp_directory_path() / "huffman_test";
        if (fs::exists(m_testDir)) {
            fs::remove_all(m_testDir);
        }
        fs::create_directories(m_testDir);
        m_sourceDir = m_testDir / "source";
        m_outputDir = m_testDir / "output";
        fs::create_directories(m_sourceDir);
        fs::create_directories(m_outputDir);
    }

    void cleanupTestCase() {
        // 删除临时目录
        if (fs::exists(m_testDir)) {
            fs::remove_all(m_testDir);
        }
    }

    // 测试：单个文本文件压缩
    void testCompressSingleTextFile() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testCompressSingleTextFile - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建测试文件
        fs::path testFile = m_sourceDir / "test.txt";
        std::string originalContent = "Hello, World! This is a test file for Huffman compression.";
        std::cout << "创建测试文件:" << testFile.string().c_str() << std::endl;
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            ofs.write(originalContent.c_str(), originalContent.size());
        }

        // 创建 DataModel 和添加文件
        Model::DataModel model;
        model.addFile(Model::FileRecord(testFile.string().c_str()));

        // 执行压缩
        fs::path outputFile = m_outputDir / "test.huff";
        std::cout << "执行压缩命令，输出文件:" << outputFile.string().c_str() << std::endl;
        Command::CompressCommand compressCmd(&model, Structure::String(outputFile.string().c_str()));
        compressCmd.execute();

        // 验证压缩文件存在
        QVERIFY(fs::exists(outputFile));
        QVERIFY(fs::file_size(outputFile) > 0);

        // 验证压缩文件格式（检查 Magic）
        char magic[5] = {0};
        {
            std::ifstream ifs(outputFile, std::ios::binary);
            ifs.read(magic, 4);
        }
        QCOMPARE(std::string(magic), std::string("HUFF"));
        
        std::cout << "✓ 测试用例: testCompressSingleTextFile - 通过" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：压缩后解压验证
    void testCompressDecompress() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testCompressDecompress - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建测试文件
        fs::path testFile = m_sourceDir / "compress_decompress.txt";
        std::string originalContent = "The quick brown fox jumps over the lazy dog. " 
                                     "Pack my box with five dozen liquor jugs. " 
                                     "AAAAAABBBBBBCCCCCCDDDDDD";
        std::cout << "创建测试文件:" << testFile.string().c_str() << std::endl;
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            ofs.write(originalContent.c_str(), originalContent.size());
        }

        // 第一步：压缩
        std::cout << "步骤 1: 执行压缩" << std::endl;
        Model::DataModel compressModel;
        compressModel.addFile(Model::FileRecord(testFile.string().c_str()));

        fs::path compressedFile = m_outputDir / "test_compress_decompress.huff";
        Command::CompressCommand compressCmd(&compressModel, Structure::String(compressedFile.string().c_str()));
        compressCmd.execute();
        std::cout << "压缩完成:" << compressedFile.string().c_str() << std::endl;

        // 验证压缩文件存在
        QVERIFY(fs::exists(compressedFile));

        // 第二步：解压
        std::cout << "步骤 2: 执行解压" << std::endl;
        Model::DataModel decompressModel;
        fs::path decompressDir = m_outputDir / "decompressed";
        fs::create_directories(decompressDir);

        Command::DecompressCommand decompressCmd(&decompressModel, 
                                                 Structure::String(compressedFile.string().c_str()),
                                                 Structure::String(decompressDir.string().c_str()));
        decompressCmd.execute();

        // 验证解压后的文件内容 (文件名是原始文件名，不是压缩文件名)
        fs::path decompressedFile = decompressDir / "compress_decompress.txt";
        QVERIFY(fs::exists(decompressedFile));

        // 读取并比较内容
        auto decompressedContent = IO::FileHandler::readBinary(Structure::String(decompressedFile.string().c_str()));
        QCOMPARE((int)decompressedContent.size(), (int)originalContent.size());

        // 验证内容一致
        std::cout << "验证解压后内容与原始内容一致" << std::endl;
        for (int i = 0; i < (int)originalContent.size(); ++i) {
            QCOMPARE(decompressedContent[i], (unsigned char)originalContent[i]);
        }
        
        std::cout << "✓ 测试用例: testCompressDecompress - 通过" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：多个文件压缩
    void testCompressMultipleFiles() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testCompressMultipleFiles - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建多个测试文件
        Structure::ArrayList<fs::path> testFiles;
        Structure::ArrayList<std::string> contents;
        std::cout << "创建 3 个测试文件" << std::endl;

        std::string file1 = "File 1: AAAAAABBBBBBCCCCCC";
        std::string file2 = "File 2: The quick brown fox";
        std::string file3 = "File 3: 1234567890";

        fs::path f1 = m_sourceDir / "file1.txt";
        fs::path f2 = m_sourceDir / "file2.txt";
        fs::path f3 = m_sourceDir / "file3.txt";

        {
            std::ofstream ofs1(f1, std::ios::binary);
            ofs1.write(file1.c_str(), file1.size());
            std::ofstream ofs2(f2, std::ios::binary);
            ofs2.write(file2.c_str(), file2.size());
            std::ofstream ofs3(f3, std::ios::binary);
            ofs3.write(file3.c_str(), file3.size());
        }

        // 创建 DataModel 和添加多个文件
        Model::DataModel model;
        model.addFile(Model::FileRecord(f1.string().c_str()));
        model.addFile(Model::FileRecord(f2.string().c_str()));
        model.addFile(Model::FileRecord(f3.string().c_str()));

        QCOMPARE(model.getFileCount(), 3);

        // 执行压缩
        fs::path outputFile = m_outputDir / "multiple.huff";
        Command::CompressCommand compressCmd(&model, Structure::String(outputFile.string().c_str()));
        compressCmd.execute();

        // 验证压缩文件
        QVERIFY(fs::exists(outputFile));

        // 验证文件格式
        char magic[5] = {0};
        int fileCount = 0;
        {
            std::ifstream ifs(outputFile, std::ios::binary);
            ifs.read(magic, 4);
            // Skip frequency table size
            int mapSize;
            ifs.read(reinterpret_cast<char*>(&mapSize), sizeof(int));
            // Skip frequency entries
            for (int i = 0; i < mapSize; ++i) {
                unsigned char c;
                int freq;
                ifs.read(reinterpret_cast<char*>(&c), 1);
                ifs.read(reinterpret_cast<char*>(&freq), sizeof(int));
            }
            // Read file count
            ifs.read(reinterpret_cast<char*>(&fileCount), sizeof(int));
        }

        QCOMPARE(std::string(magic), std::string("HUFF"));
        QCOMPARE(fileCount, 3);
        
        std::cout << "✓ 测试用例: testCompressMultipleFiles - 通过" << std::endl;
        std::cout << "压缩文件数量:" << fileCount << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：二进制文件压缩
    void testCompressBinaryFile() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testCompressBinaryFile - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建二进制测试文件
        fs::path testFile = m_sourceDir / "binary.bin";
        std::cout << "创建二进制测试文件:" << testFile.string().c_str() << std::endl;
        unsigned char binaryData[] = {0x00, 0xFF, 0x01, 0xFE, 0xAA, 0x55, 0xFF, 0x00,
                                      0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            ofs.write(reinterpret_cast<const char*>(binaryData), sizeof(binaryData));
        }

        // 创建 DataModel 和添加文件
        Model::DataModel model;
        model.addFile(Model::FileRecord(testFile.string().c_str()));

        // 执行压缩
        fs::path outputFile = m_outputDir / "binary.huff";
        Command::CompressCommand compressCmd(&model, Structure::String(outputFile.string().c_str()));
        compressCmd.execute();

        // 验证压缩文件
        QVERIFY(fs::exists(outputFile));
        QVERIFY(fs::file_size(outputFile) > 0);

        // 验证压缩文件格式
        char magic[5] = {0};
        {
            std::ifstream ifs(outputFile, std::ios::binary);
            ifs.read(magic, 4);
        }
        QCOMPARE(std::string(magic), std::string("HUFF"));
        
        std::cout << "✓ 测试用例: testCompressBinaryFile - 通过" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：单字符文件压缩解压
    void testCompressDecompressSingleChar() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testCompressDecompressSingleChar - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建包含单一字符的文件
        fs::path testFile = m_sourceDir / "single_char.txt";
        std::string content = "AAAAAAAAAA";  // 10个 A
        std::cout << "创建单字符测试文件:" << testFile.string().c_str() << std::endl;
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            ofs.write(content.c_str(), content.size());
        }

        // 压缩
        Model::DataModel compressModel;
        compressModel.addFile(Model::FileRecord(testFile.string().c_str()));

        fs::path compressedFile = m_outputDir / "single_char.huff";
        Command::CompressCommand compressCmd(&compressModel, Structure::String(compressedFile.string().c_str()));
        compressCmd.execute();

        // 解压
        Model::DataModel decompressModel;
        fs::path decompressDir = m_outputDir / "single_char_out";
        fs::create_directories(decompressDir);

        Command::DecompressCommand decompressCmd(&decompressModel,
                                                 Structure::String(compressedFile.string().c_str()),
                                                 Structure::String(decompressDir.string().c_str()));
        decompressCmd.execute();

        // 验证
        fs::path decompressedFile = decompressDir / "single_char.txt";
        QVERIFY(fs::exists(decompressedFile));

        auto decompressedContent = IO::FileHandler::readBinary(Structure::String(decompressedFile.string().c_str()));
        QCOMPARE((int)decompressedContent.size(), 10);
        
        std::cout << "验证解压内容" << std::endl;
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(decompressedContent[i], (unsigned char)'A');
        }
        
        std::cout << "✓ 测试用例: testCompressDecompressSingleChar - 通过" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：大文件压缩（生成较大的数据）
    void testCompressLargeFile() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testCompressLargeFile - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建较大的测试文件（1MB）
        fs::path testFile = m_sourceDir / "large.txt";
        const int fileSize = 1024 * 1024;  // 1MB
        std::cout << "创建大文件测试 (1MB):" << testFile.string().c_str() << std::endl;
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            // 写入重复模式
            std::string pattern = "The quick brown fox jumps over the lazy dog. ";
            for (int i = 0; i < fileSize / pattern.size(); ++i) {
                ofs.write(pattern.c_str(), pattern.size());
            }
        }

        // 创建 DataModel 和添加文件
        Model::DataModel model;
        model.addFile(Model::FileRecord(testFile.string().c_str()));

        // 执行压缩
        fs::path outputFile = m_outputDir / "large.huff";
        Command::CompressCommand compressCmd(&model, Structure::String(outputFile.string().c_str()));
        compressCmd.execute();

        // 验证压缩文件存在
        QVERIFY(fs::exists(outputFile));
        
        // 验证压缩率
        long long originalSize = fs::file_size(testFile);
        long long compressedSize = fs::file_size(outputFile);
        
        // 重复数据应该能压缩
        QVERIFY(compressedSize < originalSize);
        
        std::cout << "原始大小:" << originalSize << "字节" << std::endl;
        std::cout << "压缩大小:" << compressedSize << "字节" << std::endl;
        std::cout << "压缩率:" << (100.0 * compressedSize / originalSize) << "%" << std::endl;
        
        // 解压并验证内容
        std::cout << "解压大文件并验证内容" << std::endl;
        Model::DataModel decompressModel;
        fs::path decompressDir = m_outputDir / "large_out";
        fs::create_directories(decompressDir);
        
        Command::DecompressCommand decompressCmd(&decompressModel,
                                                 Structure::String(outputFile.string().c_str()),
                                                 Structure::String(decompressDir.string().c_str()));
        decompressCmd.execute();
        
        fs::path decompressedFile = decompressDir / "large.txt";
        QVERIFY(fs::exists(decompressedFile));
        
        // 验证文件大小
        long long decompressedSize = fs::file_size(decompressedFile);
        QCOMPARE(decompressedSize, originalSize);
        
        // 抽样验证内容（验证开头、中间、结尾）
        std::cout << "抽样验证内容正确性" << std::endl;
        auto decompressedContent = IO::FileHandler::readBinary(Structure::String(decompressedFile.string().c_str()));
        std::string pattern = "The quick brown fox jumps over the lazy dog. ";
        
        // 验证前100字节
        for (int i = 0; i < 100 && i < (int)decompressedContent.size(); ++i) {
            QCOMPARE(decompressedContent[i], (unsigned char)pattern[i % pattern.size()]);
        }
        
        // 验证后100字节
        int startIdx = decompressedContent.size() - 100;
        for (int i = startIdx; i < (int)decompressedContent.size(); ++i) {
            QCOMPARE(decompressedContent[i], (unsigned char)pattern[i % pattern.size()]);
        }
        
        std::cout << "✓ 测试用例: testCompressLargeFile - 通过" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：解压后文件数量和大小验证
    void testDecompressFileMetadata() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testDecompressFileMetadata - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建多个测试文件
        fs::path f1 = m_sourceDir / "meta1.txt";
        fs::path f2 = m_sourceDir / "meta2.txt";
        std::cout << "创建 2 个测试文件" << std::endl;
        
        std::string content1 = "Content of file 1";
        std::string content2 = "Different content for file 2";
        
        {
            std::ofstream ofs1(f1, std::ios::binary);
            ofs1.write(content1.c_str(), content1.size());
            std::ofstream ofs2(f2, std::ios::binary);
            ofs2.write(content2.c_str(), content2.size());
        }

        // 压缩
        Model::DataModel compressModel;
        compressModel.addFile(Model::FileRecord(f1.string().c_str()));
        compressModel.addFile(Model::FileRecord(f2.string().c_str()));

        fs::path compressedFile = m_outputDir / "metadata.huff";
        Command::CompressCommand compressCmd(&compressModel, Structure::String(compressedFile.string().c_str()));
        compressCmd.execute();

        // 解压
        Model::DataModel decompressModel;
        fs::path decompressDir = m_outputDir / "metadata_out";
        fs::create_directories(decompressDir);

        Command::DecompressCommand decompressCmd(&decompressModel,
                                                 Structure::String(compressedFile.string().c_str()),
                                                 Structure::String(decompressDir.string().c_str()));
        decompressCmd.execute();

        // 验证文件数量
        QCOMPARE(decompressModel.getFileCount(), 2);

        // 验证文件大小
        QCOMPARE((long long)decompressModel.getFile(0).getOriginalSize(), (long long)content1.size());
        QCOMPARE((long long)decompressModel.getFile(1).getOriginalSize(), (long long)content2.size());

        // 验证解压后的文件存在
        fs::path decompF1 = decompressDir / "meta1.txt";
        fs::path decompF2 = decompressDir / "meta2.txt";
        QVERIFY(fs::exists(decompF1));
        QVERIFY(fs::exists(decompF2));
        
        // 验证解压后的文件内容
        std::cout << "验证文件1内容" << std::endl;
        auto readContent1 = IO::FileHandler::readBinary(Structure::String(decompF1.string().c_str()));
        QCOMPARE((int)readContent1.size(), (int)content1.size());
        for (size_t i = 0; i < content1.size(); ++i) {
            QCOMPARE(readContent1[i], (unsigned char)content1[i]);
        }
        
        std::cout << "验证文件2内容" << std::endl;
        auto readContent2 = IO::FileHandler::readBinary(Structure::String(decompF2.string().c_str()));
        QCOMPARE((int)readContent2.size(), (int)content2.size());
        for (size_t i = 0; i < content2.size(); ++i) {
            QCOMPARE(readContent2[i], (unsigned char)content2[i]);
        }
        
        std::cout << "✓ 测试用例: testDecompressFileMetadata - 通过" << std::endl;
        std::cout << "文件数量:" << decompressModel.getFileCount() << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：多文件夹结构压缩与完整性验证
    void testMultiFolderCompressAndVerify() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "测试用例: testMultiFolderCompressAndVerify - 开始" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 创建复杂的文件夹结构
        fs::path folder1 = m_sourceDir / "folder1";
        fs::path folder2 = m_sourceDir / "folder2";
        fs::path subfolder = folder1 / "subfolder";
        
        fs::create_directories(folder1);
        fs::create_directories(folder2);
        fs::create_directories(subfolder);
        
        // 创建不同类型的测试文件
        std::cout << "创建多文件夹结构的测试文件" << std::endl;
        
        // folder1/file1.txt
        fs::path f1 = folder1 / "file1.txt";
        std::string content1 = "This is file 1 in folder1. It contains some test data.";
        {
            std::ofstream ofs(f1, std::ios::binary);
            ofs.write(content1.c_str(), content1.size());
        }
        
        // folder1/file2.txt
        fs::path f2 = folder1 / "file2.txt";
        std::string content2 = "File 2 content: AAABBBCCC123456789";
        {
            std::ofstream ofs(f2, std::ios::binary);
            ofs.write(content2.c_str(), content2.size());
        }
        
        // folder1/subfolder/nested.txt
        fs::path f3 = subfolder / "nested.txt";
        std::string content3 = "Nested file in subfolder with unique content!";
        {
            std::ofstream ofs(f3, std::ios::binary);
            ofs.write(content3.c_str(), content3.size());
        }
        
        // folder2/data.bin (二进制文件)
        fs::path f4 = folder2 / "data.bin";
        unsigned char binaryData[] = {0x00, 0xFF, 0xAA, 0x55, 0x12, 0x34, 0x56, 0x78};
        {
            std::ofstream ofs(f4, std::ios::binary);
            ofs.write(reinterpret_cast<const char*>(binaryData), sizeof(binaryData));
        }
        
        // folder2/readme.md
        fs::path f5 = folder2 / "readme.md";
        std::string content5 = "# README\n\nThis is a markdown file for testing.\n\n## Features\n- Feature 1\n- Feature 2";
        {
            std::ofstream ofs(f5, std::ios::binary);
            ofs.write(content5.c_str(), content5.size());
        }
        
        std::cout << "创建了 5 个文件，分布在 3 个不同的文件夹中" << std::endl;
        
        // 压缩所有文件
        std::cout << "步骤 1: 压缩所有文件" << std::endl;
        Model::DataModel compressModel;
        
        // 创建 FileRecord 并设置相对路径以保留目录结构
        Model::FileRecord record1(f1.string().c_str());
        record1.setRelativePath(Structure::String("folder1/file1.txt"));
        compressModel.addFile(record1);
        
        Model::FileRecord record2(f2.string().c_str());
        record2.setRelativePath(Structure::String("folder1/file2.txt"));
        compressModel.addFile(record2);
        
        Model::FileRecord record3(f3.string().c_str());
        record3.setRelativePath(Structure::String("folder1/subfolder/nested.txt"));
        compressModel.addFile(record3);
        
        Model::FileRecord record4(f4.string().c_str());
        record4.setRelativePath(Structure::String("folder2/data.bin"));
        compressModel.addFile(record4);
        
        Model::FileRecord record5(f5.string().c_str());
        record5.setRelativePath(Structure::String("folder2/readme.md"));
        compressModel.addFile(record5);
        
        QCOMPARE(compressModel.getFileCount(), 5);
        
        fs::path compressedFile = m_outputDir / "multi_folder.huff";
        Command::CompressCommand compressCmd(&compressModel, Structure::String(compressedFile.string().c_str()));
        compressCmd.execute();
        
        QVERIFY(fs::exists(compressedFile));
        std::cout << "压缩完成: " << compressedFile.string() << std::endl;
        
        // 计算压缩率
        long long totalOriginalSize = 0;
        totalOriginalSize += fs::file_size(f1);
        totalOriginalSize += fs::file_size(f2);
        totalOriginalSize += fs::file_size(f3);
        totalOriginalSize += fs::file_size(f4);
        totalOriginalSize += fs::file_size(f5);
        long long compressedSize = fs::file_size(compressedFile);
        
        std::cout << "原始总大小: " << totalOriginalSize << " 字节" << std::endl;
        std::cout << "压缩后大小: " << compressedSize << " 字节" << std::endl;
        std::cout << "压缩率: " << (100.0 * compressedSize / totalOriginalSize) << "%" << std::endl;
        
        // 解压所有文件
        std::cout << "\n步骤 2: 解压所有文件并验证完整性" << std::endl;
        Model::DataModel decompressModel;
        fs::path decompressDir = m_outputDir / "multi_folder_out";
        fs::create_directories(decompressDir);
        
        Command::DecompressCommand decompressCmd(&decompressModel,
                                                 Structure::String(compressedFile.string().c_str()),
                                                 Structure::String(decompressDir.string().c_str()));
        decompressCmd.execute();
        
        // 验证文件数量
        QCOMPARE(decompressModel.getFileCount(), 5);
        std::cout << "解压了 " << decompressModel.getFileCount() << " 个文件" << std::endl;
        
        // 验证每个文件的内容完整性
        std::cout << "\n步骤 3: 验证每个文件的内容完整性" << std::endl;
        
        // 验证 file1.txt
        std::cout << "验证 folder1/file1.txt" << std::endl;
        fs::path decomp_f1 = decompressDir / "folder1" / "file1.txt";
        QVERIFY(fs::exists(decomp_f1));
        auto read1 = IO::FileHandler::readBinary(Structure::String(decomp_f1.string().c_str()));
        QCOMPARE((int)read1.size(), (int)content1.size());
        for (size_t i = 0; i < content1.size(); ++i) {
            QCOMPARE(read1[i], (unsigned char)content1[i]);
        }
        
        // 验证 file2.txt
        std::cout << "验证 folder1/file2.txt" << std::endl;
        fs::path decomp_f2 = decompressDir / "folder1" / "file2.txt";
        QVERIFY(fs::exists(decomp_f2));
        auto read2 = IO::FileHandler::readBinary(Structure::String(decomp_f2.string().c_str()));
        QCOMPARE((int)read2.size(), (int)content2.size());
        for (size_t i = 0; i < content2.size(); ++i) {
            QCOMPARE(read2[i], (unsigned char)content2[i]);
        }
        
        // 验证 nested.txt
        std::cout << "验证 folder1/subfolder/nested.txt" << std::endl;
        fs::path decomp_f3 = decompressDir / "folder1" / "subfolder" / "nested.txt";
        QVERIFY(fs::exists(decomp_f3));
        auto read3 = IO::FileHandler::readBinary(Structure::String(decomp_f3.string().c_str()));
        QCOMPARE((int)read3.size(), (int)content3.size());
        for (size_t i = 0; i < content3.size(); ++i) {
            QCOMPARE(read3[i], (unsigned char)content3[i]);
        }
        
        // 验证 data.bin (二进制文件)
        std::cout << "验证 folder2/data.bin" << std::endl;
        fs::path decomp_f4 = decompressDir / "folder2" / "data.bin";
        QVERIFY(fs::exists(decomp_f4));
        auto read4 = IO::FileHandler::readBinary(Structure::String(decomp_f4.string().c_str()));
        QCOMPARE((int)read4.size(), (int)sizeof(binaryData));
        for (size_t i = 0; i < sizeof(binaryData); ++i) {
            QCOMPARE(read4[i], binaryData[i]);
        }
        
        // 验证 readme.md
        std::cout << "验证 folder2/readme.md" << std::endl;
        fs::path decomp_f5 = decompressDir / "folder2" / "readme.md";
        QVERIFY(fs::exists(decomp_f5));
        auto read5 = IO::FileHandler::readBinary(Structure::String(decomp_f5.string().c_str()));
        QCOMPARE((int)read5.size(), (int)content5.size());
        for (size_t i = 0; i < content5.size(); ++i) {
            QCOMPARE(read5[i], (unsigned char)content5[i]);
        }
        
        // 验证文件夹结构完整性
        std::cout << "\n步骤 4: 验证文件夹结构" << std::endl;
        QVERIFY(fs::exists(decompressDir / "folder1"));
        QVERIFY(fs::exists(decompressDir / "folder1" / "subfolder"));
        QVERIFY(fs::exists(decompressDir / "folder2"));
        QVERIFY(fs::is_directory(decompressDir / "folder1"));
        QVERIFY(fs::is_directory(decompressDir / "folder1" / "subfolder"));
        QVERIFY(fs::is_directory(decompressDir / "folder2"));
        std::cout << "文件夹结构验证通过" << std::endl;
        
        std::cout << "\n✓ 测试用例: testMultiFolderCompressAndVerify - 通过" << std::endl;
        std::cout << "所有文件内容和结构完整性验证通过！" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

private:
    fs::path m_testDir;
    fs::path m_sourceDir;
    fs::path m_outputDir;
};

QTEST_MAIN(TestCommand)
#include "test_command.moc"
