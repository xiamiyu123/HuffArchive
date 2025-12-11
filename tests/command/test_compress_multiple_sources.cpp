#include <QtTest>
#include "command/CompressMultipleSourcesCommand.h"
#include "command/DecompressCommand.h"
#include "model/DataModel.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

class CompressMultipleSourcesTest : public QObject {
    Q_OBJECT

private:
    fs::path testDir;
    fs::path dir1, dir2, dir2Sub;
    fs::path file1, file2, file3, file4, standalone;
    fs::path outputArchive;

    void createTestFile(const fs::path& path, const std::string& content) {
        std::ofstream file(path, std::ios::binary);
        file << content;
    }
    
    std::string readFile(const fs::path& path) {
        std::ifstream file(path, std::ios::binary);
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }

private slots:
    void initTestCase() {
        testDir = fs::temp_directory_path() / "huffman_multi_test";
        fs::create_directories(testDir);
        
        dir1 = testDir / "dir1";
        dir2 = testDir / "dir2";
        dir2Sub = dir2 / "subdir";
        
        fs::create_directories(dir1);
        fs::create_directories(dir2Sub);
        
        file1 = dir1 / "file1.txt";
        file2 = dir1 / "file2.txt";
        file3 = dir2Sub / "file3.txt";
        file4 = dir2 / "file4.txt";
        standalone = testDir / "standalone.txt";
        
        createTestFile(file1, "Content of file 1 in dir1");
        createTestFile(file2, "Content of file 2 in dir1");
        createTestFile(file3, "Content of file 3 in subdir");
        createTestFile(file4, "Content of file 4 in dir2");
        createTestFile(standalone, "Standalone file content");
        
        outputArchive = testDir / "test_archive.huff";
    }
    
    void cleanupTestCase() {
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }

    void testCompressSingleFile() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(file1.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QVERIFY(fs::exists(outputArchive));
        QCOMPARE(cmd.getModel().getFileCount(), 1);
    }

    void testCompressSingleDirectory() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(dir1.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QCOMPARE(cmd.getModel().getFileCount(), 2);
    }

    void testCompressMultipleFiles() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(file1.string().c_str()));
        sources.add(Structure::String(file3.string().c_str()));
        sources.add(Structure::String(standalone.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QCOMPARE(cmd.getModel().getFileCount(), 3);
    }

    void testCompressMultipleDirectories() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(dir1.string().c_str()));
        sources.add(Structure::String(dir2.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QCOMPARE(cmd.getModel().getFileCount(), 4);
    }

    void testCompressMixedSources() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(file1.string().c_str()));
        sources.add(Structure::String(dir2.string().c_str()));
        sources.add(Structure::String(standalone.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QCOMPARE(cmd.getModel().getFileCount(), 4);
    }

    void testCompressAndDecompressVerifyContent() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(dir1.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand compressCmd(sources, outputPath);
        QVERIFY(compressCmd.execute());
        
        // 打印压缩包中的相对路径
        std::cout << "=== Files in archive ===" << std::endl;
        for (int i = 0; i < compressCmd.getModel().getFileCount(); ++i) {
            const auto& record = compressCmd.getModel().getFile(i);
            std::cout << "Relative path: " << record.getRelativePath().c_str() << std::endl;
        }
        
        fs::path decompressDir = testDir / "decompressed";
        Structure::String decompressPath(decompressDir.string().c_str());
        
        Model::DataModel model;
        Command::DecompressCommand decompressCmd(&model, outputPath, decompressPath);
        decompressCmd.execute();
        
        // 打印解压后的实际文件
        std::cout << "=== Decompressed files ===" << std::endl;
        for (const auto& entry : fs::recursive_directory_iterator(decompressDir)) {
            if (fs::is_regular_file(entry)) {
                std::cout << "Found file: " << entry.path().string() << std::endl;
            }
        }
        
        QVERIFY(fs::exists(decompressDir / "dir1" / "file1.txt"));
        QVERIFY(fs::exists(decompressDir / "dir1" / "file2.txt"));
        
        QCOMPARE(readFile(decompressDir / "dir1" / "file1.txt"), 
                 std::string("Content of file 1 in dir1"));
        QCOMPARE(readFile(decompressDir / "dir1" / "file2.txt"), 
                 std::string("Content of file 2 in dir1"));
    }

    void testEmptySourceListShouldFail() {
        Structure::ArrayList<Structure::String> sources;
        Structure::String outputPath(outputArchive.string().c_str());
        
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        QVERIFY(!cmd.execute());
    }

    void testNonExistentSourcesSkipped() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(file1.string().c_str()));
        sources.add(Structure::String("C:/nonexistent/file.txt"));
        sources.add(Structure::String(standalone.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QCOMPARE(cmd.getModel().getFileCount(), 2);
    }

    void testNestedDirectories() {
        Structure::ArrayList<Structure::String> sources;
        sources.add(Structure::String(dir2.string().c_str()));
        
        Structure::String outputPath(outputArchive.string().c_str());
        Command::CompressMultipleSourcesCommand cmd(sources, outputPath);
        
        QVERIFY(cmd.execute());
        QCOMPARE(cmd.getModel().getFileCount(), 2);
        
        fs::path decompressDir = testDir / "decompressed_nested";
        Structure::String decompressPath(decompressDir.string().c_str());
        
        Model::DataModel model;
        Command::DecompressCommand decompressCmd(&model, outputPath, decompressPath);
        decompressCmd.execute();
        
        QVERIFY(fs::exists(decompressDir / "dir2" / "file4.txt"));
        QVERIFY(fs::exists(decompressDir / "dir2" / "subdir" / "file3.txt"));
        
        QCOMPARE(readFile(decompressDir / "dir2" / "subdir" / "file3.txt"), 
                 std::string("Content of file 3 in subdir"));
    }
};

QTEST_MAIN(CompressMultipleSourcesTest)
#include "test_compress_multiple_sources.moc"
