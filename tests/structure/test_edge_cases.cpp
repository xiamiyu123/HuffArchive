/**
 * 边界情况和异常测试
 * 这个文件包含了针对各种边界情况和异常场景的额外测试
 */

#include <QtTest>
#include "structure/String.h"
#include "structure/ArrayList.h"
#include "structure/HashMap.h"
#include "structure/PriorityQueue.h"

using namespace Structure;

class TestEdgeCases : public QObject
{
    Q_OBJECT

private slots:
    // String 边界测试
    void testStringEmpty();
    void testStringFindInEmpty();
    void testStringSubstrEdges();
    void testStringConcatenationEmpty();
    
    // ArrayList 边界测试
    void testArrayListResizeDown();
    void testArrayListEmptyOperations();
    
    // HashMap 边界测试
    void testHashMapCollisions();
    void testHashMapRehash();
    
    // PriorityQueue 边界测试
    void testPriorityQueueSameWeight();
};

// ====== String 测试 ======

void TestEdgeCases::testStringEmpty()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testStringEmpty - 空字符串操作";
    qInfo() << "========================================";
    
    String s1;
    String s2("");
    
    QVERIFY(s1.empty());
    QVERIFY(s2.empty());
    QCOMPARE(s1.length(), 0);
    QCOMPARE(s2.length(), 0);
    
    // 空字符串相等
    QVERIFY(s1 == s2);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

void TestEdgeCases::testStringFindInEmpty()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testStringFindInEmpty - 在空字符串中查找";
    qInfo() << "========================================";
    
    String s;
    QCOMPARE(s.find('a'), -1);
    QCOMPARE(s.find('z'), -1);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

void TestEdgeCases::testStringSubstrEdges()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testStringSubstrEdges - substr 边界情况";
    qInfo() << "========================================";
    
    String s = "Hello";
    
    // 越界位置
    String sub1 = s.substr(10, 5);
    QVERIFY(sub1.empty());
    
    // 负数位置
    String sub2 = s.substr(-1, 5);
    QVERIFY(sub2.empty());
    
    // 超长长度（应该截断）
    String sub3 = s.substr(2, 100);
    QCOMPARE(strcmp(sub3.c_str(), "llo"), 0);
    
    // 长度为 -1（到结尾）
    String sub4 = s.substr(2, -1);
    QCOMPARE(strcmp(sub4.c_str(), "llo"), 0);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

void TestEdgeCases::testStringConcatenationEmpty()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testStringConcatenationEmpty - 空字符串连接";
    qInfo() << "========================================";
    
    String s1 = "Hello";
    String s2;
    
    // 非空 + 空
    String s3 = s1 + s2;
    QCOMPARE(strcmp(s3.c_str(), "Hello"), 0);
    
    // 空 + 非空
    String s4 = s2 + s1;
    QCOMPARE(strcmp(s4.c_str(), "Hello"), 0);
    
    // 空 + 空
    String s5 = s2 + String();
    QVERIFY(s5.empty());
    
    // += 空
    s1 += s2;
    QCOMPARE(strcmp(s1.c_str(), "Hello"), 0);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

// ====== ArrayList 测试 ======

void TestEdgeCases::testArrayListResizeDown()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testArrayListResizeDown - ArrayList 缩小";
    qInfo() << "========================================";
    
    ArrayList<int> list;
    list.add(1);
    list.add(2);
    list.add(3);
    list.add(4);
    list.add(5);
    
    QCOMPARE(list.size(), 5);
    
    // 缩小到 3
    list.resize(3);
    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0], 1);
    QCOMPARE(list[1], 2);
    QCOMPARE(list[2], 3);
    
    // 缩小到 0
    list.resize(0);
    QCOMPARE(list.size(), 0);
    QVERIFY(list.empty());
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

void TestEdgeCases::testArrayListEmptyOperations()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testArrayListEmptyOperations - 空列表操作";
    qInfo() << "========================================";
    
    ArrayList<int> list;
    
    // 空列表 pop_back
    QVERIFY(list.empty());
    list.pop_back();  // 应该安全（不崩溃）
    QVERIFY(list.empty());
    
    // 空列表访问
    QVERIFY_EXCEPTION_THROWN(list.front(), std::out_of_range);
    QVERIFY_EXCEPTION_THROWN(list.back(), std::out_of_range);
    
    // clear 空列表
    list.clear();
    QVERIFY(list.empty());
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

// ====== HashMap 测试 ======

void TestEdgeCases::testHashMapCollisions()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testHashMapCollisions - 哈希冲突处理";
    qInfo() << "========================================";
    
    // 创建小容量 HashMap 以增加冲突概率
    HashMap<int, int> map(4);
    
    // 插入多个元素（可能产生冲突）
    for (int i = 0; i < 20; ++i) {
        map.put(i, i * 10);
    }
    
    // 验证所有元素都能正确获取
    for (int i = 0; i < 20; ++i) {
        QVERIFY(map.contains(i));
        QCOMPARE(map[i], i * 10);
    }
    
    QCOMPARE(map.size(), 20);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

void TestEdgeCases::testHashMapRehash()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testHashMapRehash - 自动扩容";
    qInfo() << "========================================";
    
    HashMap<int, int> map(8);
    
    // 插入足够多的元素触发 rehash
    // LOAD_FACTOR = 0.75, 初始容量 8, 应该在 6 个元素后扩容
    for (int i = 0; i < 100; ++i) {
        map.put(i, i);
    }
    
    // 验证所有元素依然可访问
    for (int i = 0; i < 100; ++i) {
        QVERIFY(map.contains(i));
        QCOMPARE(map[i], i);
    }
    
    QCOMPARE(map.size(), 100);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

// ====== PriorityQueue 测试 ======

void TestEdgeCases::testPriorityQueueSameWeight()
{
    qInfo() << "\n========================================";
    qInfo() << "测试用例: testPriorityQueueSameWeight - 相同权重元素";
    qInfo() << "========================================";
    
    PriorityQueue<int> pq;
    
    // 插入多个相同值
    pq.push(5);
    pq.push(5);
    pq.push(5);
    pq.push(3);
    pq.push(7);
    
    QCOMPARE(pq.size(), 5);
    
    // 应该先弹出最小的
    QCOMPARE(pq.top(), 3);
    pq.pop();
    
    // 然后是 5（可能有多个）
    QCOMPARE(pq.top(), 5);
    pq.pop();
    QCOMPARE(pq.top(), 5);
    pq.pop();
    QCOMPARE(pq.top(), 5);
    pq.pop();
    
    // 最后是 7
    QCOMPARE(pq.top(), 7);
    
    qInfo() << "✓ 测试通过";
    qInfo() << "========================================\n";
}

QTEST_APPLESS_MAIN(TestEdgeCases)
#include "test_edge_cases.moc"
