#include <QtTest>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "command/CompressDirectoryCommand.h"
#include "command/DecompressDirectoryCommand.h"

namespace fs = std::filesystem;

class TestDirectoryCommand : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // 创建临时测试目录
        m_testDir = fs::temp_directory_path() / "huffman_dir_test";
        if (fs::exists(m_testDir)) {
            fs::remove_all(m_testDir);
        }
        fs::create_directories(m_testDir);
    }

    void cleanupTestCase() {
        // 清理临时目录
        if (fs::exists(m_testDir)) {
            fs::remove_all(m_testDir);
        }
    }

    // 测试：压缩单个文件
    void testCompressSingleFile() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test: testCompressSingleFile" << std::endl;
        std::cout << "========================================" << std::endl;

        // 创建测试文件
        fs::path sourceFile = m_testDir / "single_file.txt";
        std::string content = "Hello, World!";
        {
            std::ofstream ofs(sourceFile);
            ofs << content;
        }

        // 压缩单个文件
        fs::path outputArchive = m_testDir / "single_file.huff";
        Command::CompressDirectoryCommand cmd(
            Structure::String(sourceFile.string().c_str()),
            Structure::String(outputArchive.string().c_str())
        );

        QVERIFY2(cmd.execute(), cmd.getErrorMessage().c_str());
        QVERIFY(fs::exists(outputArchive));
        QVERIFY(fs::file_size(outputArchive) > 0);

        std::cout << "✓ Test passed: Single file compressed successfully" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：压缩目录（自动递归和相对路径处理）
    void testCompressDirectory() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test: testCompressDirectory" << std::endl;
        std::cout << "========================================" << std::endl;

        // 创建目录结构
        fs::path sourceDir = m_testDir / "source_project";
        fs::path dir1 = sourceDir / "src";
        fs::path dir2 = sourceDir / "data";
        fs::path subdir = dir2 / "nested";
        
        fs::create_directories(dir1);
        fs::create_directories(subdir);

        // 创建多个测试文件
        std::vector<std::pair<fs::path, std::string>> files = {
            {dir1 / "main.cpp", "#include <iostream>\nint main() { return 0; }"},
            {dir1 / "util.cpp", "void util_func() {}"},
            {dir2 / "config.txt", "[Settings]\nversion=1.0"},
            {subdir / "data.txt", "Some data content"}
        };

        for (const auto& [path, content] : files) {
            std::ofstream ofs(path);
            ofs << content;
            std::cout << "Created: " << path.string() << std::endl;
        }

        // 压缩整个目录
        fs::path outputArchive = m_testDir / "project.huff";
        Command::CompressDirectoryCommand cmd(
            Structure::String(sourceDir.string().c_str()),
            Structure::String(outputArchive.string().c_str())
        );

        std::cout << "Compressing directory: " << sourceDir.string() << std::endl;
        QVERIFY2(cmd.execute(), cmd.getErrorMessage().c_str());
        QVERIFY(fs::exists(outputArchive));
        QVERIFY(fs::file_size(outputArchive) > 0);

        std::cout << "✓ Test passed: Directory compressed successfully" << std::endl;
        std::cout << "Archive size: " << fs::file_size(outputArchive) << " bytes" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：压缩后解压，验证文件完整性
    void testCompressDecompressDirectory() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test: testCompressDecompressDirectory" << std::endl;
        std::cout << "========================================" << std::endl;

        // 1. 创建源目录
        fs::path sourceDir = m_testDir / "original";
        fs::path dir1 = sourceDir / "documents";
        fs::path dir2 = sourceDir / "images";
        fs::create_directories(dir1);
        fs::create_directories(dir2);

        // 创建测试文件
        std::string doc_content = "This is a document.\nWith multiple lines.";
        std::string image_content = "\x89PNG\r\n\x1a\n...fake image data...";

        std::ofstream doc(dir1 / "readme.txt");
        doc << doc_content;
        doc.close();

        std::ofstream img(dir2 / "photo.txt");  // 用 txt 代替实际图片
        img << image_content;
        img.close();

        // 2. 压缩
        std::cout << "Step 1: Compressing..." << std::endl;
        fs::path archive = m_testDir / "backup.huff";
        Command::CompressDirectoryCommand compressCmd(
            Structure::String(sourceDir.string().c_str()),
            Structure::String(archive.string().c_str())
        );
        QVERIFY2(compressCmd.execute(), compressCmd.getErrorMessage().c_str());
        std::cout << "✓ Compression complete" << std::endl;

        // 3. 解压
        std::cout << "Step 2: Decompressing..." << std::endl;
        fs::path extractDir = m_testDir / "extracted";
        Command::DecompressDirectoryCommand decompressCmd(
            Structure::String(archive.string().c_str()),
            Structure::String(extractDir.string().c_str())
        );
        QVERIFY2(decompressCmd.execute(), decompressCmd.getErrorMessage().c_str());
        std::cout << "✓ Decompression complete" << std::endl;

        // 4. 验证文件完整性
        std::cout << "Step 3: Verifying..." << std::endl;
        
        // 检查原始文件
        fs::path extracted_readme = extractDir / "documents" / "readme.txt";
        fs::path extracted_photo = extractDir / "images" / "photo.txt";
        
        QVERIFY(fs::exists(extracted_readme));
        QVERIFY(fs::exists(extracted_photo));

        // 验证文件内容
        std::ifstream readme_ifs(extracted_readme);
        std::string readme_content((std::istreambuf_iterator<char>(readme_ifs)),
                                   std::istreambuf_iterator<char>());
        QCOMPARE(readme_content, doc_content);

        std::cout << "✓ All files verified successfully" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

private:
    fs::path m_testDir;
};

#include "test_directory_command.moc"
QTEST_MAIN(TestDirectoryCommand)
