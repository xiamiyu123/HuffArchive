#include <QtTest>
#include <filesystem>
#include <fstream>
#include <random>
#include "command/CompressDirectoryCommand.h"
#include "command/DecompressDirectoryCommand.h"
#include "structure/String.h"

namespace fs = std::filesystem;

class TestFullCycle : public QObject
{
    Q_OBJECT

private:
    fs::path m_testRoot;
    fs::path m_sourceDir;
    fs::path m_outputDir;
    fs::path m_archivePath;

    void createRandomFile(const fs::path& path, size_t size) {
        std::ofstream ofs(path, std::ios::binary);
        std::mt19937 gen(12345); // Fixed seed for reproducibility
        std::uniform_int_distribution<> dis(0, 255);
        
        for (size_t i = 0; i < size; ++i) {
            char c = static_cast<char>(dis(gen));
            ofs.write(&c, 1);
        }
    }

    bool compareFiles(const fs::path& p1, const fs::path& p2) {
        if (!fs::exists(p1) || !fs::exists(p2)) return false;
        if (fs::file_size(p1) != fs::file_size(p2)) return false;

        std::ifstream f1(p1, std::ios::binary);
        std::ifstream f2(p2, std::ios::binary);
        
        return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                          std::istreambuf_iterator<char>(),
                          std::istreambuf_iterator<char>(f2.rdbuf()));
    }

    bool compareDirectories(const fs::path& d1, const fs::path& d2) {
        for (const auto& entry : fs::recursive_directory_iterator(d1)) {
            fs::path relPath = fs::relative(entry.path(), d1);
            fs::path p2 = d2 / relPath;
            
            if (!fs::exists(p2)) {
                qDebug() << "Missing file in restored:" << p2.string().c_str();
                return false;
            }
            
            if (fs::is_regular_file(entry.path())) {
                if (!compareFiles(entry.path(), p2)) {
                    qDebug() << "Content mismatch:" << relPath.string().c_str();
                    return false;
                }
            } else if (fs::is_directory(entry.path())) {
                if (!fs::is_directory(p2)) {
                    qDebug() << "Type mismatch (expected dir):" << p2.string().c_str();
                    return false;
                }
            }
        }
        return true;
    }

private slots:
    void initTestCase() {
        m_testRoot = fs::temp_directory_path() / "huffman_integration_test";
        if (fs::exists(m_testRoot)) fs::remove_all(m_testRoot);
        
        m_sourceDir = m_testRoot / "source";
        m_outputDir = m_testRoot / "restored";
        m_archivePath = m_testRoot / "archive.huff";
        
        fs::create_directories(m_sourceDir);
        fs::create_directories(m_outputDir);
    }

    void cleanupTestCase() {
        if (fs::exists(m_testRoot)) fs::remove_all(m_testRoot);
    }

    void testComplexDirectoryCycle() {
        // 1. Setup complex directory structure
        fs::create_directories(m_sourceDir / "subdir1");
        fs::create_directories(m_sourceDir / "subdir2" / "nested");
        
        createRandomFile(m_sourceDir / "root.bin", 1024 * 10); // 10KB
        createRandomFile(m_sourceDir / "subdir1" / "file1.txt", 500);
        createRandomFile(m_sourceDir / "subdir2" / "nested" / "deep.dat", 2048);
        
        // Empty file
        std::ofstream(m_sourceDir / "empty.txt");
        
        // 2. Compress
        Command::CompressDirectoryCommand compressCmd(
            Structure::String(m_sourceDir.string().c_str()),
            Structure::String(m_archivePath.string().c_str())
        );
        QVERIFY(compressCmd.execute());
        QVERIFY(fs::exists(m_archivePath));
        
        // 3. Decompress
        Command::DecompressDirectoryCommand decompressCmd(
            Structure::String(m_archivePath.string().c_str()),
            Structure::String(m_outputDir.string().c_str())
        );
        QVERIFY(decompressCmd.execute());
        
        // 4. Verify
        // Note: DecompressDirectoryCommand usually extracts into the output dir.
        // If source was "source/", it might extract as "output/source/" or just contents into "output/".
        // Assuming it preserves the root folder name if it was compressed as a folder.
        // Let's check if "source" exists in output, or if contents are directly in output.
        // Based on typical behavior, if we compressed "source", we expect "source" inside "restored".
        
        fs::path restoredRoot = m_outputDir;
        if (fs::exists(m_outputDir / "source")) {
            restoredRoot = m_outputDir / "source";
        }
        
        QVERIFY(compareDirectories(m_sourceDir, restoredRoot));
    }
};

QTEST_MAIN(TestFullCycle)
#include "test_full_cycle.moc"
