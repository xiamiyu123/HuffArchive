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
    String s1;
    QVERIFY(s1.empty());
    QCOMPARE(s1.length(), 0);

    String s2("Hello");
    QCOMPARE(s2.length(), 5);
    QCOMPARE(strcmp(s2.c_str(), "Hello"), 0);

    String s3(s2);
    QCOMPARE(s3.length(), 5);
    QCOMPARE(strcmp(s3.c_str(), "Hello"), 0);
}

void TestString::testAssignment()
{
    String s1 = "Hello";
    String s2;
    s2 = s1;
    QCOMPARE(s2.length(), 5);
    QCOMPARE(strcmp(s2.c_str(), "Hello"), 0);

    s2 = "World";
    QCOMPARE(strcmp(s2.c_str(), "World"), 0);
}

void TestString::testConcatenation()
{
    String s1 = "Hello";
    String s2 = " World";
    String s3 = s1 + s2;
    
    QCOMPARE(strcmp(s3.c_str(), "Hello World"), 0);

    s1 += "!";
    QCOMPARE(strcmp(s1.c_str(), "Hello!"), 0);
}

void TestString::testComparison()
{
    String s1 = "Apple";
    String s2 = "Banana";
    String s3 = "Apple";

    QVERIFY(s1 == s3);
    QVERIFY(s1 != s2);
    QVERIFY(s1 < s2);
    QVERIFY(s1 == "Apple");
}

void TestString::testAccess()
{
    String s = "Hello";
    QCOMPARE(s[0], 'H');
    QCOMPARE(s[4], 'o');
    
    s[0] = 'h';
    QCOMPARE(s[0], 'h');
}

void TestString::testSubstr()
{
    String s = "Hello World";
    String sub = s.substr(6, 5);
    QCOMPARE(strcmp(sub.c_str(), "World"), 0);
    
    String sub2 = s.substr(0, 5);
    QCOMPARE(strcmp(sub2.c_str(), "Hello"), 0);
}

void TestString::testFind()
{
    String s = "Hello World";
    QCOMPARE(s.find('W'), 6);
    QCOMPARE(s.find('l'), 2);
    QCOMPARE(s.find('z'), -1);
}

QTEST_APPLESS_MAIN(TestString)
#include "test_string.moc"
