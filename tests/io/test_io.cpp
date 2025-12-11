#include <QtTest>
#include <iostream>
#include <QString>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include "io/BitStream.h"
#include "io/FileHandler.h"
#include "structure/ArrayList.h"

class TestIO : public QObject
{
    Q_OBJECT

private slots:
    void testBitStream();
    void testFileHandlerText();
    void testFileHandlerBinary();
};

void TestIO::testBitStream()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testBitStream - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    IO::BitStream bs;
    
    // Test 1: Write bits manually
    std::cout << "测试 1: 手动写入位" << std::endl;
    bs.writeBit(1);
    bs.writeBit(0);
    bs.writeBit(1);
    // Current byte: 10100000 (0xA0) if flushed now, but count is 3
    
    // Write 5 more bits to fill the byte
    bs.writeBit(1);
    bs.writeBit(1);
    bs.writeBit(0);
    bs.writeBit(0);
    bs.writeBit(1);
    // Byte: 10111001 = 0xB9
    
    auto bytes = bs.getBytes();
    QCOMPARE(bytes.size(), 1);
    QCOMPARE(static_cast<int>(bytes[0]), 0xB9);
    
    // Test 2: Write bits from string
    std::cout << "测试 2: 从字符串写入位" << std::endl;
    bs.clear();
    bs.writeBits("11110000"); // 0xF0
    bytes = bs.getBytes();
    QCOMPARE(bytes.size(), 1);
    QCOMPARE(static_cast<int>(bytes[0]), 0xF0);
    
    // Test 3: Read bits
    std::cout << "测试 3: 读取位" << std::endl;
    bs.loadBytes(bytes);
    QCOMPARE(bs.readBit(), 1);
    QCOMPARE(bs.readBit(), 1);
    QCOMPARE(bs.readBit(), 1);
    QCOMPARE(bs.readBit(), 1);
    QCOMPARE(bs.readBit(), 0);
    QCOMPARE(bs.readBit(), 0);
    QCOMPARE(bs.readBit(), 0);
    QCOMPARE(bs.readBit(), 0);
    QCOMPARE(bs.readBit(), -1); // EOF
    
    std::cout << "✓ 测试用例: testBitStream - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestIO::testFileHandlerText()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testFileHandlerText - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    QTemporaryFile file;
    if (file.open()) {
        Structure::String path = file.fileName().toStdString().c_str();
        Structure::String content = "Hello, World!\nTesting FileHandler.";
        
        // Write
        bool success = IO::FileHandler::writeText(path, content);
        QVERIFY(success);
        
        // Read
        std::cout << "读取文本文件" << std::endl;
        Structure::String readContent = IO::FileHandler::readText(path);
        QCOMPARE(readContent, content);
        
        std::cout << "✓ 测试用例: testFileHandlerText - 通过" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}

void TestIO::testFileHandlerBinary()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testFileHandlerBinary - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    QTemporaryFile file;
    if (file.open()) {
        Structure::String path = file.fileName().toStdString().c_str();
        Structure::ArrayList<unsigned char> data;
        data.add(0x00);
        data.add(0xFF);
        data.add(0xAA);
        data.add(0x55);
        
        // Write
        bool success = IO::FileHandler::writeBinary(path, data);
        QVERIFY(success);
        
        // Read
        std::cout << "读取二进制文件" << std::endl;
        auto readData = IO::FileHandler::readBinary(path);
        QCOMPARE(readData.size(), data.size());
        for (int i = 0; i < data.size(); ++i) {
            QCOMPARE(readData[i], data[i]);
        }
        
        std::cout << "✓ 测试用例: testFileHandlerBinary - 通过" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}

QTEST_MAIN(TestIO)
#include "test_io.moc"
