#include <QtTest>
#include <iostream>
#include "structure/LLRBTree.h"
#include "structure/String.h"

using namespace Structure;

class TestLLRBTree : public QObject
{
    Q_OBJECT

private slots:
    void testPutAndGet();
    void testContains();
    void testUpdate();
    void testLargeData();
};

void TestLLRBTree::testPutAndGet()
{
    LLRBTree<int, int> tree;
    tree.put(1, 10);
    tree.put(2, 20);
    tree.put(3, 30);

    QVERIFY(tree.get(1) != nullptr);
    QCOMPARE(*tree.get(1), 10);
    
    QVERIFY(tree.get(2) != nullptr);
    QCOMPARE(*tree.get(2), 20);
    
    QVERIFY(tree.get(3) != nullptr);
    QCOMPARE(*tree.get(3), 30);
    
    QVERIFY(tree.get(4) == nullptr);
}

void TestLLRBTree::testContains()
{
    LLRBTree<int, int> tree;
    tree.put(5, 50);
    QVERIFY(tree.contains(5));
    QVERIFY(!tree.contains(10));
}

void TestLLRBTree::testUpdate()
{
    LLRBTree<int, int> tree;
    tree.put(1, 10);
    QCOMPARE(*tree.get(1), 10);
    
    tree.put(1, 100);
    QCOMPARE(*tree.get(1), 100);
}

void TestLLRBTree::testLargeData()
{
    LLRBTree<int, int> tree;
    int n = 1000;
    for (int i = 0; i < n; ++i) {
        tree.put(i, i * 2);
    }
    
    for (int i = 0; i < n; ++i) {
        int* val = tree.get(i);
        QVERIFY(val != nullptr);
        QCOMPARE(*val, i * 2);
    }
}

QTEST_APPLESS_MAIN(TestLLRBTree)

#include "test_llrbtree.moc"
