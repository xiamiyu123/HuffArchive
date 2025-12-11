#include <QtTest>
#include "structure/String.h"
#include <sstream>

using namespace Structure;

class TestString : public QObject
{
    Q_OBJECT

private slots:
    void testConstruction();
    void testAssignment();
    void testConcatenation();
    void testComparison();
    void testAccess();
    void testSubstr();
    void testFind();
};

void TestString::testConstruction()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testConstruction - 开始";
    qInfo() << "========================================";
    
    String s1;
    QVERIFY(s1.empty());
    QCOMPARE(s1.length(), 0);

    String s2("Hello");
    QCOMPARE(s2.length(), 5);
    QCOMPARE(strcmp(s2.c_str(), "Hello"), 0);

    String s3(s2);
    QCOMPARE(s3.length(), 5);
    QCOMPARE(strcmp(s3.c_str(), "Hello"), 0);
    
    qInfo() << "✓ 测试用例: testConstruction - 通过";
    qInfo() << "========================================\n";
}

void TestString::testAssignment()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testAssignment - 开始";
    qInfo() << "========================================";
    
    String s1 = "Hello";
    String s2;
    s2 = s1;
    QCOMPARE(s2.length(), 5);
    QCOMPARE(strcmp(s2.c_str(), "Hello"), 0);

    s2 = "World";
    QCOMPARE(strcmp(s2.c_str(), "World"), 0);
    
    qInfo() << "✓ 测试用例: testAssignment - 通过";
    qInfo() << "========================================\n";
}

void TestString::testConcatenation()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testConcatenation - 开始";
    qInfo() << "========================================";
    
    String s1 = "Hello";
    String s2 = " World";
    String s3 = s1 + s2;
    
    QCOMPARE(strcmp(s3.c_str(), "Hello World"), 0);

    s1 += "!";
    QCOMPARE(strcmp(s1.c_str(), "Hello!"), 0);
    
    qInfo() << "✓ 测试用例: testConcatenation - 通过";
    qInfo() << "========================================\n";
}

void TestString::testComparison()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testComparison - 开始";
    qInfo() << "========================================";
    
    String s1 = "Apple";
    String s2 = "Banana";
    String s3 = "Apple";

    QVERIFY(s1 == s3);
    QVERIFY(s1 != s2);
    QVERIFY(s1 < s2);
    QVERIFY(s1 == "Apple");
    
    qInfo() << "✓ 测试用例: testComparison - 通过";
    qInfo() << "========================================\n";
}

void TestString::testAccess()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testAccess - 开始";
    qInfo() << "========================================";
    
    String s = "Hello";
    QCOMPARE(s[0], 'H');
    QCOMPARE(s[4], 'o');
    
    s[0] = 'h';
    QCOMPARE(s[0], 'h');
    
    qInfo() << "✓ 测试用例: testAccess - 通过";
    qInfo() << "========================================\n";
}

void TestString::testSubstr()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testSubstr - 开始";
    qInfo() << "========================================";
    
    String s = "Hello World";
    String sub = s.substr(6, 5);
    QCOMPARE(strcmp(sub.c_str(), "World"), 0);
    
    String sub2 = s.substr(0, 5);
    QCOMPARE(strcmp(sub2.c_str(), "Hello"), 0);
    
    qInfo() << "✓ 测试用例: testSubstr - 通过";
    qInfo() << "========================================\n";
}

void TestString::testFind()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testFind - 开始";
    qInfo() << "========================================";
    
    String s = "Hello World";
    QCOMPARE(s.find('W'), 6);
    QCOMPARE(s.find('l'), 2);
    QCOMPARE(s.find('z'), -1);
    
    qInfo() << "✓ 测试用例: testFind - 通过";
    qInfo() << "========================================\n";
}

QTEST_APPLESS_MAIN(TestString)
#include "test_string.moc"
