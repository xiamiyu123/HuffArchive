#include <QtTest>
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
        // 创建测试文件
        fs::path testFile = m_sourceDir / "test.txt";
        std::string originalContent = "Hello, World! This is a test file for Huffman compression.";
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            ofs.write(originalContent.c_str(), originalContent.size());
        }

        // 创建 DataModel 和添加文件
        Model::DataModel model;
        model.addFile(Model::FileRecord(testFile.string().c_str()));

        // 执行压缩
        fs::path outputFile = m_outputDir / "test.huff";
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
    }

    // 测试：压缩后解压验证
    void testCompressDecompress() {
        // 创建测试文件
        fs::path testFile = m_sourceDir / "compress_decompress.txt";
        std::string originalContent = "The quick brown fox jumps over the lazy dog. " 
                                     "Pack my box with five dozen liquor jugs. " 
                                     "AAAAAABBBBBBCCCCCCDDDDDD";
        
        {
            std::ofstream ofs(testFile, std::ios::binary);
            ofs.write(originalContent.c_str(), originalContent.size());
        }

        // 第一步：压缩
        Model::DataModel compressModel;
        compressModel.addFile(Model::FileRecord(testFile.string().c_str()));

        fs::path compressedFile = m_outputDir / "test_compress_decompress.huff";
        Command::CompressCommand compressCmd(&compressModel, Structure::String(compressedFile.string().c_str()));
        compressCmd.execute();

        // 验证压缩文件存在
        QVERIFY(fs::exists(compressedFile));

        // 第二步：解压
        Model::DataModel decompressModel;
        fs::path decompressDir = m_outputDir / "decompressed";
        fs::create_directories(decompressDir);

        Command::DecompressCommand decompressCmd(&decompressModel, 
                                                 Structure::String(compressedFile.string().c_str()),
                                                 Structure::String(decompressDir.string().c_str()));
        decompressCmd.execute();

        // 验证解压后的文件内容
        fs::path decompressedFile = decompressDir / "test_compress_decompress.txt";
        QVERIFY(fs::exists(decompressedFile));

        // 读取并比较内容
        auto decompressedContent = IO::FileHandler::readBinary(Structure::String(decompressedFile.string().c_str()));
        QCOMPARE((int)decompressedContent.size(), (int)originalContent.size());

        // 验证内容一致
        for (int i = 0; i < (int)originalContent.size(); ++i) {
            QCOMPARE(decompressedContent[i], (unsigned char)originalContent[i]);
        }
    }

    // 测试：多个文件压缩
    void testCompressMultipleFiles() {
        // 创建多个测试文件
        Structure::ArrayList<fs::path> testFiles;
        Structure::ArrayList<std::string> contents;

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
    }

    // 测试：二进制文件压缩
    void testCompressBinaryFile() {
        // 创建二进制测试文件
        fs::path testFile = m_sourceDir / "binary.bin";
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
    }

    // 测试：单字符文件压缩解压
    void testCompressDecompressSingleChar() {
        // 创建包含单一字符的文件
        fs::path testFile = m_sourceDir / "single_char.txt";
        std::string content = "AAAAAAAAAA";  // 10个 A
        
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
        
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(decompressedContent[i], (unsigned char)'A');
        }
    }

    // 测试：大文件压缩（生成较大的数据）
    void testCompressLargeFile() {
        // 创建较大的测试文件（1MB）
        fs::path testFile = m_sourceDir / "large.txt";
        const int fileSize = 1024 * 1024;  // 1MB
        
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
        
        qDebug() << "Original size:" << originalSize << "bytes";
        qDebug() << "Compressed size:" << compressedSize << "bytes";
        qDebug() << "Compression ratio:" << (100.0 * compressedSize / originalSize) << "%";
    }

    // 测试：解压后文件数量和大小验证
    void testDecompressFileMetadata() {
        // 创建多个测试文件
        fs::path f1 = m_sourceDir / "meta1.txt";
        fs::path f2 = m_sourceDir / "meta2.txt";
        
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
    }

private:
    fs::path m_testDir;
    fs::path m_sourceDir;
    fs::path m_outputDir;
};

QTEST_MAIN(TestCommand)
#include "test_command.moc"
