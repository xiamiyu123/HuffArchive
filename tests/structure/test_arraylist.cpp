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
    void testMoveSemantics();
    void testInsert();
    void testSwap();
    void testSearch();
    void testCapacity();
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

void TestArrayList::testMoveSemantics()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testMoveSemantics - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    // Move Constructor
    ArrayList<int> list1;
    list1.add(10);
    list1.add(20);
    
    ArrayList<int> list2(std::move(list1));
    QCOMPARE(list2.size(), 2);
    QCOMPARE(list2.get(0), 10);
    QCOMPARE(list1.size(), 0); // list1 should be empty
    QCOMPARE(list1.data(), nullptr);

    // Move Assignment
    ArrayList<int> list3;
    list3.add(30);
    list3 = std::move(list2);
    QCOMPARE(list3.size(), 2);
    QCOMPARE(list3.get(0), 10);
    QCOMPARE(list2.size(), 0);

    std::cout << "✓ 测试用例: testMoveSemantics - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testInsert()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testInsert - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    ArrayList<int> list;
    list.add(10);
    list.add(30);
    
    list.insert(1, 20); // Insert 20 at index 1
    QCOMPARE(list.size(), 3);
    QCOMPARE(list.get(0), 10);
    QCOMPARE(list.get(1), 20);
    QCOMPARE(list.get(2), 30);
    
    list.insert(0, 5); // Insert at beginning
    QCOMPARE(list.get(0), 5);
    
    list.insert(4, 40); // Insert at end
    QCOMPARE(list.get(4), 40);

    QVERIFY_EXCEPTION_THROWN(list.insert(100, 0), std::out_of_range);

    std::cout << "✓ 测试用例: testInsert - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testSwap()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testSwap - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    ArrayList<int> list1;
    list1.add(1);
    
    ArrayList<int> list2;
    list2.add(2);
    list2.add(3);
    
    list1.swap(list2);
    
    QCOMPARE(list1.size(), 2);
    QCOMPARE(list1.get(0), 2);
    QCOMPARE(list2.size(), 1);
    QCOMPARE(list2.get(0), 1);

    std::cout << "✓ 测试用例: testSwap - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testSearch()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testSearch - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    ArrayList<int> list;
    list.add(10);
    list.add(20);
    list.add(30);
    
    QCOMPARE(list.indexOf(20), 1);
    QCOMPARE(list.indexOf(40), -1);
    
    QVERIFY(list.contains(10));
    QVERIFY(!list.contains(50));

    std::cout << "✓ 测试用例: testSearch - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestArrayList::testCapacity()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testCapacity - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    ArrayList<int> list;
    QCOMPARE(list.capacity(), 0);
    
    list.reserve(100);
    QVERIFY(list.capacity() >= 100);
    
    list.add(1);
    list.shrink_to_fit();
    QCOMPARE(list.capacity(), 1);
    QCOMPARE(list.size(), 1);
    
    list.clear();
    list.shrink_to_fit();
    QCOMPARE(list.capacity(), 0);

    std::cout << "✓ 测试用例: testCapacity - 通过" << std::endl;
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
