#include <QtTest>
#include <iostream>
#include "structure/TreeMapSTL.h"
#include "structure/String.h"

using namespace Structure;

class TestTreeMapSTL : public QObject
{
    Q_OBJECT

private slots:
    void testPutAndGet();
    void testContains();
    void testSize();
    void testTraverse();
};

void TestTreeMapSTL::testPutAndGet()
{
    TreeMapSTL<int, String> map;
    map.put(1, "One");
    map.put(2, "Two");
    
    QVERIFY(map.contains(1));
    QCOMPARE(*map.get(1), String("One"));
    QCOMPARE(*map.get(2), String("Two"));
    QVERIFY(!map.contains(3));
    QVERIFY(map.get(3) == nullptr);
    
    // Update
    map.put(1, "OneUpdated");
    QCOMPARE(*map.get(1), String("OneUpdated"));
}

void TestTreeMapSTL::testContains()
{
    TreeMapSTL<int, int> map;
    map.put(5, 50);
    QVERIFY(map.contains(5));
    QVERIFY(!map.contains(10));
}

void TestTreeMapSTL::testSize()
{
    TreeMapSTL<int, int> map;
    QVERIFY(map.isEmpty());
    QCOMPARE(map.size(), 0);
    
    map.put(1, 1);
    QVERIFY(!map.isEmpty());
    QCOMPARE(map.size(), 1);
    
    map.put(2, 2);
    QCOMPARE(map.size(), 2);
    
    map.put(1, 10); // Update
    QCOMPARE(map.size(), 2);
}

void TestTreeMapSTL::testTraverse()
{
    TreeMapSTL<int, int> map;
    map.put(3, 30);
    map.put(1, 10);
    map.put(2, 20);
    
    int count = 0;
    int sumKeys = 0;
    int sumValues = 0;
    
    map.traverse([&](const int& key, const int& value) {
        count++;
        sumKeys += key;
        sumValues += value;
    });
    
    QCOMPARE(count, 3);
    QCOMPARE(sumKeys, 1 + 2 + 3);
    QCOMPARE(sumValues, 10 + 20 + 30);
}

QTEST_APPLESS_MAIN(TestTreeMapSTL)

#include "test_treemapstl.moc"
