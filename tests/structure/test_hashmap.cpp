#include <QtTest>
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
    void testIterator();
    void testStringKey();
};

void TestHashMap::testPutAndGet()
{
    HashMap<int, int> map;
    map.put(1, 100);
    map.put(2, 200);

    QCOMPARE(map[1], 100);
    QCOMPARE(map[2], 200);
    QCOMPARE(map.size(), 2);
}

void TestHashMap::testContains()
{
    HashMap<int, int> map;
    map.put(1, 10);

    QVERIFY(map.contains(1));
    QVERIFY(!map.contains(2));
}

void TestHashMap::testUpdate()
{
    HashMap<int, int> map;
    map.put(1, 10);
    map.put(1, 20); // Update

    QCOMPARE(map[1], 20);
    QCOMPARE(map.size(), 1);
}

void TestHashMap::testIterator()
{
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
}

void TestHashMap::testStringKey()
{
    HashMap<String, int> map;
    map.put("one", 1);
    map.put("two", 2);

    QCOMPARE(map["one"], 1);
    QCOMPARE(map["two"], 2);
    QVERIFY(map.contains("one"));
}

QTEST_APPLESS_MAIN(TestHashMap)
#include "test_hashmap.moc"
