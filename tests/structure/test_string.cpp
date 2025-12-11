#include <QtTest>
#include "structure/String.h"
#include <sstream>
#include <iostream>

using namespace Structure;

class TestString : public QObject
{
    Q_OBJECT

private slots:
    void testConstruction();
    void testAssignment();
    void testConcatenation();
    void testComparison();
    void testExtendedComparison();
    void testAccess();
    void testElementAccess();
    void testModification();
    void testSubstr();
    void testFind();
};

void TestString::testConstruction()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testConstruction - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    String s1;
    QVERIFY(s1.empty());
    QCOMPARE(s1.length(), 0);

    String s2("Hello");
    QCOMPARE(s2.length(), 5);
    QCOMPARE(strcmp(s2.c_str(), "Hello"), 0);

    String s3(s2);
    QCOMPARE(s3.length(), 5);
    QCOMPARE(strcmp(s3.c_str(), "Hello"), 0);
    
    std::cout << "✓ 测试用例: testConstruction - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestString::testAssignment()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testAssignment - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    String s1 = "Hello";
    String s2;
    s2 = s1;
    QCOMPARE(s2.length(), 5);
    QCOMPARE(strcmp(s2.c_str(), "Hello"), 0);

    s2 = "World";
    QCOMPARE(strcmp(s2.c_str(), "World"), 0);
    
    std::cout << "✓ 测试用例: testAssignment - 通过";
    std::cout << "========================================\n";
}

void TestString::testConcatenation()
{
    std::cout << "\n========================================";
    std::cout << "测试用例: testConcatenation - 开始";
    std::cout << "========================================";
    
    String s1 = "Hello";
    String s2 = " World";
    String s3 = s1 + s2;
    
    QCOMPARE(strcmp(s3.c_str(), "Hello World"), 0);

    s1 += "!";
    QCOMPARE(strcmp(s1.c_str(), "Hello!"), 0);
    
    std::cout << "✓ 测试用例: testConcatenation - 通过";
    std::cout << "========================================\n";
}

void TestString::testComparison()
{
    std::cout << "\n========================================";
    std::cout << "测试用例: testComparison - 开始";
    std::cout << "========================================";
    
    String s1 = "Apple";
    String s2 = "Banana";
    String s3 = "Apple";

    QVERIFY(s1 == s3);
    QVERIFY(s1 != s2);
    QVERIFY(s1 < s2);
    QVERIFY(s1 == "Apple");
    
    std::cout << "✓ 测试用例: testComparison - 通过";
    std::cout << "========================================\n";
}

void TestString::testAccess()
{
    std::cout << "\n========================================";
    std::cout << "测试用例: testAccess - 开始";
    std::cout << "========================================";
    
    String s = "Hello";
    QCOMPARE(s[0], 'H');
    QCOMPARE(s[4], 'o');
    
    s[0] = 'h';
    QCOMPARE(s[0], 'h');
    
    std::cout << "✓ 测试用例: testAccess - 通过";
    std::cout << "========================================\n";
}

void TestString::testSubstr()
{
    std::cout << "\n========================================";
    std::cout << "测试用例: testSubstr - 开始";
    std::cout << "========================================";
    
    String s = "Hello World";
    String sub = s.substr(6, 5);
    QCOMPARE(strcmp(sub.c_str(), "World"), 0);
    
    String sub2 = s.substr(0, 5);
    QCOMPARE(strcmp(sub2.c_str(), "Hello"), 0);
    
    std::cout << "✓ 测试用例: testSubstr - 通过";
    std::cout << "========================================\n";
}

void TestString::testFind()
{
    std::cout << "\n========================================";
    std::cout << "测试用例: testFind - 开始";
    std::cout << "========================================";
    
    String s = "Hello World";
    QCOMPARE(s.find('W'), 6);
    QCOMPARE(s.find('l'), 2);
    QCOMPARE(s.find('z'), -1);
    
    std::cout << "✓ 测试用例: testFind - 通过";
    std::cout << "========================================\n";
}

void TestString::testExtendedComparison()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testExtendedComparison - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    String s1 = "Apple";
    String s2 = "Banana";
    String s3 = "Apple";

    QVERIFY(s2 > s1);
    QVERIFY(s1 <= s3);
    QVERIFY(s1 >= s3);
    QVERIFY(s2 >= s1);
    
    std::cout << "✓ 测试用例: testExtendedComparison - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestString::testElementAccess()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testElementAccess - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    String s = "Hello";
    
    // Test at()
    QCOMPARE(s.at(0), 'H');
    QCOMPARE(s.at(4), 'o');
    
    bool exceptionCaught = false;
    try {
        s.at(5);
    } catch (const std::out_of_range&) {
        exceptionCaught = true;
    }
    QVERIFY(exceptionCaught);

    // Test front() and back()
    QCOMPARE(s.front(), 'H');
    QCOMPARE(s.back(), 'o');
    
    // Modify via references
    s.front() = 'h';
    s.back() = 'O';
    QCOMPARE(strcmp(s.c_str(), "hellO"), 0);

    std::cout << "✓ 测试用例: testElementAccess - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestString::testModification()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testModification - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    String s = "Hello";
    
    // Test push_back
    s.push_back('!');
    QCOMPARE(strcmp(s.c_str(), "Hello!"), 0);
    QCOMPARE(s.length(), 6);

    // Test pop_back
    s.pop_back();
    QCOMPARE(strcmp(s.c_str(), "Hello"), 0);
    QCOMPARE(s.length(), 5);
    
    // Test clear
    s.clear();
    QVERIFY(s.empty());
    QCOMPARE(s.length(), 0);
    
    // Test swap
    String s1 = "ABC";
    String s2 = "DEF";
    s1.swap(s2);
    QCOMPARE(strcmp(s1.c_str(), "DEF"), 0);
    QCOMPARE(strcmp(s2.c_str(), "ABC"), 0);

    std::cout << "✓ 测试用例: testModification - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

QTEST_APPLESS_MAIN(TestString)
#include "test_string.moc"

