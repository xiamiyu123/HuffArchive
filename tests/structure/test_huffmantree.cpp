#include <QtTest>
#include "structure/HuffmanTree.h"
#include "structure/HashMap.h"
#include "structure/String.h"
#include "structure/ArrayList.h"
#include <iostream>
#include <vector>

class TestHuffmanTree : public QObject
{
    Q_OBJECT

private slots:
    void testBuildAndCodes();
    void testEncodeDecode();
    void testStreamDecode();
};

void TestHuffmanTree::testBuildAndCodes()
{
    Structure::HuffmanTree tree;
    Structure::HashMap<unsigned char, int> frequencies;
    
    // Simple case: A: 5, B: 9, C: 12, D: 13, E: 16, F: 45
    frequencies.put('A', 5);
    frequencies.put('B', 9);
    frequencies.put('C', 12);
    frequencies.put('D', 13);
    frequencies.put('E', 16);
    frequencies.put('F', 45);
    
    tree.build(frequencies);
    
    auto codes = tree.generateCodes();
    
    // Verify all characters have codes
    QVERIFY(codes.containsKey('A'));
    QVERIFY(codes.containsKey('B'));
    QVERIFY(codes.containsKey('C'));
    QVERIFY(codes.containsKey('D'));
    QVERIFY(codes.containsKey('E'));
    QVERIFY(codes.containsKey('F'));
    
    // Verify prefix property (no code is a prefix of another)
    auto keys = codes.keys();
    for (int i = 0; i < keys.size(); ++i) {
        Structure::String code1 = codes.get(keys[i]);
        for (int j = 0; j < keys.size(); ++j) {
            if (i == j) continue;
            Structure::String code2 = codes.get(keys[j]);
            QVERIFY(!code2.startsWith(code1));
        }
    }
    
    // Verify F (highest frequency) has the shortest code (or equal shortest)
    // In standard Huffman, F should be length 1 ('0' or '1')
    // But depending on implementation details, we just check lengths generally
    int lenF = codes.get('F').length();
    int lenA = codes.get('A').length();
    QVERIFY(lenF <= lenA);
}

void TestHuffmanTree::testEncodeDecode()
{
    Structure::HuffmanTree tree;
    Structure::HashMap<unsigned char, int> frequencies;
    
    Structure::String text = "this is a test string for huffman encoding";
    for (int i = 0; i < text.length(); ++i) {
        unsigned char c = text[i];
        if (frequencies.containsKey(c)) {
            frequencies.put(c, frequencies.get(c) + 1);
        } else {
            frequencies.put(c, 1);
        }
    }
    
    tree.build(frequencies);
    
    // Prepare data for encoding
    Structure::ArrayList<unsigned char> data;
    for (int i = 0; i < text.length(); ++i) {
        data.add(text[i]);
    }
    
    // Encode
    Structure::String encoded = tree.encode(data);
    QVERIFY(encoded.length() > 0);
    
    // Decode
    Structure::ArrayList<unsigned char> decoded = tree.decode(encoded);
    
    // Verify
    QCOMPARE(decoded.size(), data.size());
    Structure::String decodedStr;
    for (int i = 0; i < decoded.size(); ++i) {
        decodedStr += (char)decoded[i];
    }
    QCOMPARE(decodedStr, text);
}

void TestHuffmanTree::testStreamDecode()
{
    Structure::HuffmanTree tree;
    Structure::HashMap<unsigned char, int> frequencies;
    frequencies.put('a', 10);
    frequencies.put('b', 5);
    
    tree.build(frequencies);
    
    // 'a' might be '0', 'b' might be '1' (or vice versa)
    auto codes = tree.generateCodes();
    Structure::String codeA = codes.get('a');
    Structure::String codeB = codes.get('b');
    
    // Construct a bit stream for "aba"
    Structure::String bitString = codeA + codeB + codeA;
    
    std::vector<int> bits;
    for (int i = 0; i < bitString.length(); ++i) {
        bits.push_back(bitString[i] == '1' ? 1 : 0);
    }
    
    int bitIndex = 0;
    auto readBit = [&]() -> int {
        if (bitIndex < bits.size()) {
            return bits[bitIndex++];
        }
        return -1;
    };
    
    Structure::String decodedStr;
    auto writeByte = [&](unsigned char b) {
        decodedStr += (char)b;
    };
    
    tree.decode(readBit, writeByte, 3); // Target size 3 ("aba")
    
    QCOMPARE(decodedStr.c_str(), "aba");
}

QTEST_MAIN(TestHuffmanTree)
#include "test_huffmantree.moc"
