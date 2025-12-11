#include <QtTest>
#include <iostream>
#include "structure/HashMap.h"
#include "structure/String.h"

using namespace Structure;

class TestHashMap : public QObject
{
    Q_OBJECT

private slots:
    void testPutAndGet();
    void testContains();
    void testUpdate();
    void testRemove();
    void testAt();
    void testSwap();
    void testMoveSemantics();
    void testIterator();
    void testStringKey();
};

void TestHashMap::testPutAndGet()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testPutAndGet - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    HashMap<int, int> map;
    map.put(1, 100);
    map.put(2, 200);

    QCOMPARE(map[1], 100);
    QCOMPARE(map[2], 200);
    QCOMPARE(map.size(), 2);
    
    std::cout << "✓ 测试用例: testPutAndGet - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testContains()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testContains - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    HashMap<int, int> map;
    map.put(1, 10);

    QVERIFY(map.contains(1));
    QVERIFY(!map.contains(2));
    
    std::cout << "✓ 测试用例: testContains - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testUpdate()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testUpdate - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    HashMap<int, int> map;
    map.put(1, 10);
    map.put(1, 20); // Update

    QCOMPARE(map[1], 20);
    QCOMPARE(map.size(), 1);
    
    std::cout << "✓ 测试用例: testUpdate - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testRemove()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testRemove - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    HashMap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);
    
    QVERIFY(map.remove(1));
    QVERIFY(!map.contains(1));
    QCOMPARE(map.size(), 1);
    
    QVERIFY(!map.remove(3)); // Remove non-existent
    QCOMPARE(map.size(), 1);

    std::cout << "✓ 测试用例: testRemove - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testAt()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testAt - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    HashMap<int, int> map;
    map.put(1, 10);
    
    QCOMPARE(map.at(1), 10);
    
    bool exceptionCaught = false;
    try {
        map.at(2);
    } catch (const std::out_of_range&) {
        exceptionCaught = true;
    }
    QVERIFY(exceptionCaught);

    std::cout << "✓ 测试用例: testAt - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testSwap()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testSwap - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    HashMap<int, int> map1;
    map1.put(1, 10);
    
    HashMap<int, int> map2;
    map2.put(2, 20);
    
    map1.swap(map2);
    
    QVERIFY(map1.contains(2));
    QVERIFY(!map1.contains(1));
    QVERIFY(map2.contains(1));
    QVERIFY(!map2.contains(2));

    std::cout << "✓ 测试用例: testSwap - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testMoveSemantics()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testMoveSemantics - 开始" << std::endl;
    std::cout << "========================================" << std::endl;

    // Move Constructor
    HashMap<int, int> map1;
    map1.put(1, 10);
    
    HashMap<int, int> map2(std::move(map1));
    QVERIFY(map2.contains(1));
    QCOMPARE(map2.size(), 1);
    QCOMPARE(map1.size(), 0); // map1 should be empty

    // Move Assignment
    HashMap<int, int> map3;
    map3 = std::move(map2);
    QVERIFY(map3.contains(1));
    QCOMPARE(map3.size(), 1);
    QCOMPARE(map2.size(), 0);

    std::cout << "✓ 测试用例: testMoveSemantics - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testIterator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testIterator - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    HashMap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);
    map.put(3, 30);

    int count = 0;
    int sumKeys = 0;
    int sumValues = 0;

    for (auto it = map.begin(); it != map.end(); ++it) {
        count++;
        sumKeys += it->first;
        sumValues += it->second;
    }

    QCOMPARE(count, 3);
    QCOMPARE(sumKeys, 6);
    QCOMPARE(sumValues, 60);
    
    std::cout << "✓ 测试用例: testIterator - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void TestHashMap::testStringKey()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试用例: testStringKey - 开始" << std::endl;
    std::cout << "========================================" << std::endl;
    
    HashMap<String, int> map;
    map.put("one", 1);
    map.put("two", 2);

    QCOMPARE(map["one"], 1);
    QCOMPARE(map["two"], 2);
    QVERIFY(map.contains("one"));
    
    std::cout << "✓ 测试用例: testStringKey - 通过" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

QTEST_APPLESS_MAIN(TestHashMap)
#include "test_hashmap.moc"
