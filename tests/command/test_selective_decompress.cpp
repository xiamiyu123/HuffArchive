#include <QtTest>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "command/CompressDirectoryCommand.h"
#include "command/SelectiveDecompressCommand.h"

namespace fs = std::filesystem;

class TestSelectiveDecompressCommand : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_testDir = fs::temp_directory_path() / "huffman_selective_test";
        if (fs::exists(m_testDir)) {
            fs::remove_all(m_testDir);
        }
        fs::create_directories(m_testDir);
    }

    void cleanupTestCase() {
        if (fs::exists(m_testDir)) {
            fs::remove_all(m_testDir);
        }
    }

    // 测试：选择性解压 - 只解压特定文件
    void testSelectiveDecompress() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test: testSelectiveDecompress" << std::endl;
        std::cout << "========================================" << std::endl;

        // 1. 创建源目录和文件
        fs::path sourceDir = m_testDir / "source_files";
        fs::path srcDir = sourceDir / "src";
        fs::path dataDir = sourceDir / "data";
        fs::path configDir = sourceDir / "config";
        
        fs::create_directories(srcDir);
        fs::create_directories(dataDir);
        fs::create_directories(configDir);

        // 创建测试文件 - 使用二进制模式以避免换行符问题
        std::map<std::string, std::string> files = {
            {"src/main.cpp", "#include <iostream>\nint main() { return 0; }"},
            {"src/util.cpp", "void util() {}"},
            {"src/util.h", "void util();"},
            {"data/input.txt", "Input data content"},
            {"data/output.txt", "Output data content"},
            {"config/settings.ini", "[Settings]\nversion=1.0\nmode=release"}
        };

        for (const auto& [relPath, content] : files) {
            fs::path filePath = sourceDir / relPath;
            std::ofstream ofs(filePath, std::ios::binary);
            ofs.write(content.c_str(), content.size());
            std::cout << "Created: " << relPath << std::endl;
        }

        // 2. 压缩整个目录
        std::cout << "\nStep 1: Compressing directory..." << std::endl;
        fs::path archive = m_testDir / "archive.huff";
        Command::CompressDirectoryCommand compressCmd(
            Structure::String(sourceDir.string().c_str()),
            Structure::String(archive.string().c_str())
        );
        QVERIFY2(compressCmd.execute(), compressCmd.getErrorMessage().c_str());
        std::cout << "✓ Archive created: " << archive.string() << std::endl;

        // 3. 创建选择性解压列表（只解压源文件）
        std::cout << "\nStep 2: Preparing selective extraction filter..." << std::endl;
        Structure::ArrayList<Structure::String> filterList;
        filterList.add(Structure::String("src/main.cpp"));
        filterList.add(Structure::String("src/util.h"));
        filterList.add(Structure::String("config/settings.ini"));
        
        std::cout << "Files to extract:" << std::endl;
        for (int i = 0; i < filterList.size(); ++i) {
            std::cout << "  - " << filterList[i].c_str() << std::endl;
        }

        // 4. 执行选择性解压
        std::cout << "\nStep 3: Performing selective decompression..." << std::endl;
        fs::path extractDir = m_testDir / "extracted";
        Command::SelectiveDecompressCommand decompressCmd(
            Structure::String(archive.string().c_str()),
            Structure::String(extractDir.string().c_str()),
            filterList
        );
        
        QVERIFY2(decompressCmd.execute(), decompressCmd.getErrorMessage().c_str());
        std::cout << "✓ Extraction complete" << std::endl;
        std::cout << "  Extracted files: " << decompressCmd.getExtractedCount() << std::endl;
        std::cout << "  Skipped files: " << decompressCmd.getSkippedCount() << std::endl;

        // 5. 验证提取的文件
        std::cout << "\nStep 4: Verifying extracted files..." << std::endl;
        
        // 应该存在的文件
        fs::path main_cpp = extractDir / "src" / "main.cpp";
        fs::path util_h = extractDir / "src" / "util.h";
        fs::path settings_ini = extractDir / "config" / "settings.ini";
        
        QVERIFY2(fs::exists(main_cpp), "main.cpp should be extracted");
        QVERIFY2(fs::exists(util_h), "util.h should be extracted");
        QVERIFY2(fs::exists(settings_ini), "settings.ini should be extracted");
        
        std::cout << "✓ All filtered files extracted successfully" << std::endl;

        // 不应该存在的文件
        fs::path util_cpp = extractDir / "src" / "util.cpp";
        fs::path input_txt = extractDir / "data" / "input.txt";
        fs::path output_txt = extractDir / "data" / "output.txt";
        
        QVERIFY2(!fs::exists(util_cpp), "util.cpp should NOT be extracted");
        QVERIFY2(!fs::exists(input_txt), "input.txt should NOT be extracted");
        QVERIFY2(!fs::exists(output_txt), "output.txt should NOT be extracted");
        
        std::cout << "✓ Unfiltered files correctly excluded" << std::endl;

        // 验证文件内容
        std::cout << "\nStep 5: Verifying file contents..." << std::endl;
        {
            std::ifstream main_ifs(main_cpp, std::ios::binary);
            std::string main_content((std::istreambuf_iterator<char>(main_ifs)),
                                     std::istreambuf_iterator<char>());
            
            if (main_content != files["src/main.cpp"]) {
                std::cout << "Content mismatch!" << std::endl;
                std::cout << "Expected size: " << files["src/main.cpp"].size() << std::endl;
                std::cout << "Actual size: " << main_content.size() << std::endl;
            }
            QCOMPARE(main_content, files["src/main.cpp"]);
        }
        
        std::cout << "✓ File contents verified" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "✓ Test passed: Selective decompression works correctly" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：空过滤列表（应解压所有文件）
    void testSelectiveDecompressEmpty() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test: testSelectiveDecompressEmpty" << std::endl;
        std::cout << "========================================" << std::endl;

        // 1. 创建和压缩文件
        fs::path sourceDir = m_testDir / "source_all";
        fs::create_directories(sourceDir);
        
        {
            std::ofstream file1(sourceDir / "file1.txt", std::ios::binary);
            file1 << "Content 1";
        }
        
        {
            std::ofstream file2(sourceDir / "file2.txt", std::ios::binary);
            file2 << "Content 2";
        }

        std::cout << "Created test files" << std::endl;

        fs::path archive = m_testDir / "archive_all.huff";
        Command::CompressDirectoryCommand compressCmd(
            Structure::String(sourceDir.string().c_str()),
            Structure::String(archive.string().c_str())
        );
        QVERIFY2(compressCmd.execute(), compressCmd.getErrorMessage().c_str());
        std::cout << "✓ Archive created" << std::endl;

        // 2. 使用空过滤列表解压（应该解压所有文件）
        std::cout << "\nExtracting with empty filter (should extract all)..." << std::endl;
        Structure::ArrayList<Structure::String> emptyFilter;
        fs::path extractDir = m_testDir / "extracted_all";
        
        Command::SelectiveDecompressCommand decompressCmd(
            Structure::String(archive.string().c_str()),
            Structure::String(extractDir.string().c_str()),
            emptyFilter
        );
        
        QVERIFY2(decompressCmd.execute(), decompressCmd.getErrorMessage().c_str());
        std::cout << "✓ Extraction complete" << std::endl;
        std::cout << "  Extracted files: " << decompressCmd.getExtractedCount() << std::endl;

        // 3. 验证所有文件都被提取
        QVERIFY(fs::exists(extractDir / "file1.txt"));
        QVERIFY(fs::exists(extractDir / "file2.txt"));
        
        std::cout << "✓ All files extracted with empty filter" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 测试：提取非空文件和空文件
    void testSelectiveDecompressWithEmptyFiles() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test: testSelectiveDecompressWithEmptyFiles" << std::endl;
        std::cout << "========================================" << std::endl;

        // 1. 创建包含空文件的目录
        fs::path sourceDir = m_testDir / "source_mixed";
        fs::create_directories(sourceDir);
        
        {
            std::ofstream file1(sourceDir / "content.txt", std::ios::binary);
            file1 << "Some content";
        }
        
        {
            std::ofstream file2(sourceDir / "empty.txt", std::ios::binary);
            // 创建空文件
        }

        std::cout << "Created files (1 normal, 1 empty)" << std::endl;

        fs::path archive = m_testDir / "archive_mixed.huff";
        Command::CompressDirectoryCommand compressCmd(
            Structure::String(sourceDir.string().c_str()),
            Structure::String(archive.string().c_str())
        );
        QVERIFY2(compressCmd.execute(), compressCmd.getErrorMessage().c_str());

        // 2. 只提取空文件
        std::cout << "\nExtracting only empty file..." << std::endl;
        Structure::ArrayList<Structure::String> filterList;
        filterList.add(Structure::String("empty.txt"));
        
        fs::path extractDir = m_testDir / "extracted_empty";
        Command::SelectiveDecompressCommand decompressCmd(
            Structure::String(archive.string().c_str()),
            Structure::String(extractDir.string().c_str()),
            filterList
        );
        
        QVERIFY2(decompressCmd.execute(), decompressCmd.getErrorMessage().c_str());
        
        // 3. 验证
        fs::path emptyFile = extractDir / "empty.txt";
        QVERIFY(fs::exists(emptyFile));
        QVERIFY(fs::file_size(emptyFile) == 0);
        QVERIFY(!fs::exists(extractDir / "content.txt"));
        
        std::cout << "✓ Empty file correctly extracted and other files excluded" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

private:
    fs::path m_testDir;
};

#include "test_selective_decompress.moc"
QTEST_MAIN(TestSelectiveDecompressCommand)
