#include <QtTest>
#include <iostream>
#include "structure/TreeMap.h"
#include "structure/String.h"

using namespace Structure;

class TestTreeMap : public QObject
{
    Q_OBJECT

private slots:
    void testPutAndGet();
    void testOperatorBracket();
    void testKeys();
    void testSize();
};

void TestTreeMap::testPutAndGet()
{
    TreeMap<int, String> map;
    map.put(1, "One");
    map.put(2, "Two");
    
    QVERIFY(map.contains(1));
    QCOMPARE(*map.get(1), String("One"));
    QCOMPARE(*map.get(2), String("Two"));
    QVERIFY(!map.contains(3));
}

void TestTreeMap::testOperatorBracket()
{
    TreeMap<int, int> map;
    map[1] = 100;
    map[2] = 200;
    
    QCOMPARE(map[1], 100);
    QCOMPARE(map[2], 200);
    
    map[1] = 101;
    QCOMPARE(map[1], 101);
    
    // Test default insertion
    QCOMPARE(map[3], 0); // Assuming int default constructs to 0
}

void TestTreeMap::testKeys()
{
    TreeMap<int, int> map;
    map.put(3, 30);
    map.put(1, 10);
    map.put(2, 20);
    
    ArrayList<int> keys = map.keys();
    QCOMPARE(keys.size(), 3);
    QCOMPARE(keys[0], 1);
    QCOMPARE(keys[1], 2);
    QCOMPARE(keys[2], 3);
}

void TestTreeMap::testSize()
{
    TreeMap<int, int> map;
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

QTEST_APPLESS_MAIN(TestTreeMap)

#include "test_treemap.moc"
