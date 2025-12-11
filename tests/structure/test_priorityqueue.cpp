#include <QtTest>
#include "structure/PriorityQueue.h"

using namespace Structure;

class TestPriorityQueue : public QObject
{
    Q_OBJECT

private slots:
    void testMinHeap();
    void testMaxHeap();
    void testCustomComparator();
    void testEmpty();
};

void TestPriorityQueue::testMinHeap()
{
    // Default is min heap (std::less)
    PriorityQueue<int> pq;
    pq.push(30);
    pq.push(10);
    pq.push(20);

    QCOMPARE(pq.size(), 3);
    QCOMPARE(pq.top(), 10);
    
    pq.pop();
    QCOMPARE(pq.top(), 20);
    
    pq.pop();
    QCOMPARE(pq.top(), 30);
    
    pq.pop();
    QVERIFY(pq.empty());
}

void TestPriorityQueue::testMaxHeap()
{
    // Use std::greater for max heap
    PriorityQueue<int, std::greater<int>> pq;
    pq.push(10);
    pq.push(30);
    pq.push(20);

    QCOMPARE(pq.top(), 30);
    
    pq.pop();
    QCOMPARE(pq.top(), 20);
    
    pq.pop();
    QCOMPARE(pq.top(), 10);
}

struct CustomStruct {
    int id;
    int priority;
    
    bool operator==(const CustomStruct& other) const {
        return id == other.id && priority == other.priority;
    }
};

// Custom comparator: higher priority first
struct CustomComparator {
    bool operator()(const CustomStruct& a, const CustomStruct& b) const {
        return a.priority > b.priority; // Note: PriorityQueue logic: if comp(child, parent) is true, swap. 
                                        // If we want max heap behavior (larger priority at top), 
                                        // we need child > parent to trigger swap.
                                        // Wait, let's check PriorityQueue implementation.
                                        // swim: if (m_comp(m_data[index], m_data[parent])) swap
                                        // If we want smaller value at top (MinHeap), we use std::less (a < b).
                                        // If child < parent, swap. Correct.
                                        // If we want larger value at top (MaxHeap), we use std::greater (a > b).
                                        // If child > parent, swap. Correct.
                                        // So for CustomStruct, if we want higher priority at top, we need a > b logic.
        return a.priority > b.priority;
    }
};

void TestPriorityQueue::testCustomComparator()
{
    PriorityQueue<CustomStruct, CustomComparator> pq;
    pq.push({1, 10});
    pq.push({2, 30});
    pq.push({3, 20});

    QCOMPARE(pq.top().priority, 30);
    pq.pop();
    QCOMPARE(pq.top().priority, 20);
    pq.pop();
    QCOMPARE(pq.top().priority, 10);
}

void TestPriorityQueue::testEmpty()
{
    PriorityQueue<int> pq;
    QVERIFY(pq.empty());
    QVERIFY_EXCEPTION_THROWN(pq.top(), std::out_of_range);
    
    pq.push(1);
    pq.pop();
    QVERIFY(pq.empty());
}

QTEST_APPLESS_MAIN(TestPriorityQueue)
#include "test_priorityqueue.moc"
