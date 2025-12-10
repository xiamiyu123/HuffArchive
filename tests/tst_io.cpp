#include <QtTest>
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
    IO::BitStream bs;
    
    // Test 1: Write bits manually
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
    QCOMPARE(bytes[0], 0xB9);
    
    // Test 2: Write bits from string
    bs.clear();
    bs.writeBits("11110000"); // 0xF0
    bytes = bs.getBytes();
    QCOMPARE(bytes.size(), 1);
    QCOMPARE(bytes[0], 0xF0);
    
    // Test 3: Read bits
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
}

void TestIO::testFileHandlerText()
{
    QTemporaryFile file;
    if (file.open()) {
        std::string path = file.fileName().toStdString();
        std::string content = "Hello, World!\nTesting FileHandler.";
        
        // Write
        bool success = IO::FileHandler::writeText(path, content);
        QVERIFY(success);
        
        // Read
        std::string readContent = IO::FileHandler::readText(path);
        QCOMPARE(readContent, content);
    }
}

void TestIO::testFileHandlerBinary()
{
    QTemporaryFile file;
    if (file.open()) {
        std::string path = file.fileName().toStdString();
        Structure::ArrayList<unsigned char> data;
        data.add(0x00);
        data.add(0xFF);
        data.add(0xAA);
        data.add(0x55);
        
        // Write
        bool success = IO::FileHandler::writeBinary(path, data);
        QVERIFY(success);
        
        // Read
        auto readData = IO::FileHandler::readBinary(path);
        QCOMPARE(readData.size(), data.size());
        for (int i = 0; i < data.size(); ++i) {
            QCOMPARE(readData[i], data[i]);
        }
    }
}

QTEST_MAIN(TestIO)
#include "tst_io.moc"
