#include <QtTest>
#include <iostream>
#include "structure/ArrayList.h"

using namespace Structure;

class TestArrayList : public QObject
{
    Q_OBJECT

private slots:
    void testAddAndGet();
    void testRemove();
    void testCopyConstructor();
    void testAssignmentOperator();
    void testOutOfBounds();
};

void TestArrayList::testAddAndGet()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testAddAndGet - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ArrayList<int> list;
    QCOMPARE(list.size(), 0);

    list.add(10);
    list.add(20);
    list.add(30);

    QCOMPARE(list.size(), 3);
    QCOMPARE(list.get(0), 10);
    QCOMPARE(list.get(1), 20);
    QCOMPARE(list.get(2), 30);
    QCOMPARE(list[1], 20);
    
    std::cout << "✓ 测试用例: testAddAndGet - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testRemove()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testRemove - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ArrayList<int> list;
    list.add(1);
    list.add(2);
    list.add(3);

    list.remove(1); // Remove '2'

    QCOMPARE(list.size(), 2);
    QCOMPARE(list.get(0), 1);
    QCOMPARE(list.get(1), 3);
    
    std::cout << "✓ 测试用例: testRemove - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testCopyConstructor()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testCopyConstructor - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ArrayList<int> list1;
    list1.add(100);
    list1.add(200);

    ArrayList<int> list2(list1);

    QCOMPARE(list2.size(), 2);
    QCOMPARE(list2.get(0), 100);
    QCOMPARE(list2.get(1), 200);

    // Modify list2, list1 should not change
    list2.add(300);
    QCOMPARE(list1.size(), 2);
    QCOMPARE(list2.size(), 3);
    
    std::cout << "✓ 测试用例: testCopyConstructor - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testAssignmentOperator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testAssignmentOperator - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ArrayList<int> list1;
    list1.add(1);
    
    ArrayList<int> list2;
    list2.add(2);
    list2.add(3);

    list1 = list2;

    QCOMPARE(list1.size(), 2);
    QCOMPARE(list1.get(0), 2);
    QCOMPARE(list1.get(1), 3);
    
    std::cout << "✓ 测试用例: testAssignmentOperator - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testOutOfBounds()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testOutOfBounds - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ArrayList<int> list;
    list.add(1);

    QVERIFY_EXCEPTION_THROWN(list.get(1), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(list.get(-1), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(list.remove(1), std::out_of_range);
    
    std::cout << "✓ 测试用例: testOutOfBounds - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

QTEST_APPLESS_MAIN(TestArrayList)
#include "test_arraylist.moc"
