#include <QtTest>
#include <iostream>
#include "structure/TreeMap.h"
#include "structure/HashMap.h"
#include "structure/MapHuff.h"
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
    void testPolymorphism();
    void testTraverse();
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

void TestTreeMap::testPolymorphism()
{
    TreeMap<int, int> treeMap;
    HashMap<int, int> hashMap;
    
    MapHuff<int, int>* maps[] = { &treeMap, &hashMap };
    
    for (auto map : maps) {
        map->put(1, 100);
        QVERIFY(map->contains(1));
        QCOMPARE(*map->get(1), 100);
        QCOMPARE(map->size(), 1);
        QVERIFY(!map->isEmpty());
    }
}

void TestTreeMap::testTraverse()
{
    TreeMap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);
    map.put(3, 30);
    
    int count = 0;
    int sumKeys = 0;
    int sumValues = 0;
    
    map.traverse([&](const int& k, const int& v) {
        count++;
        sumKeys += k;
        sumValues += v;
    });
    
    QCOMPARE(count, 3);
    QCOMPARE(sumKeys, 6);
    QCOMPARE(sumValues, 60);
}

QTEST_APPLESS_MAIN(TestTreeMap)

#include "test_treemap.moc"
