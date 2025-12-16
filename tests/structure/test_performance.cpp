#include <QtTest>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <QDebug>
#include "structure/MapFactory.h"
#include "structure/String.h"

using namespace Structure;

class TestPerformance : public QObject
{
    Q_OBJECT

private slots:
    void comparePerformance();
};

void TestPerformance::comparePerformance()
{
    const int DATA_SIZE = 100000; // Data size
    std::cout << "\n========================================" << std::endl;
    std::cout << "Performance Comparison: HashMap vs TreeMap (Data Size:" << DATA_SIZE << ")" << std::endl;
    std::cout << "========================================" << std::endl;

    // Prepare random data
    std::vector<int> keys(DATA_SIZE);
    std::vector<int> values(DATA_SIZE);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 10000000);

    for (int i = 0; i < DATA_SIZE; ++i) {
        keys[i] = dist(rng);
        values[i] = dist(rng);
    }

    // Test HashMap
    {
        MapHuff<int, int>* map = MapFactory<int, int>::createMap(MapType::HASH_MAP);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->put(keys[i], values[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> insertTime = end - start;
        
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->get(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> getTime = end - start;

        std::cout << "[HashMap] Insert Time:" << insertTime.count() << "ms" << std::endl;
        std::cout << "[HashMap] Get Time:" << getTime.count() << "ms" << std::endl;

        delete map;
    }

    // Test TreeMap
    {
        MapHuff<int, int>* map = MapFactory<int, int>::createMap(MapType::TREE_MAP);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->put(keys[i], values[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> insertTime = end - start;
        
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->get(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> getTime = end - start;

        std::cout << "[TreeMap] Insert Time:" << insertTime.count() << "ms" << std::endl;
        std::cout << "[TreeMap] Get Time:" << getTime.count() << "ms" << std::endl;

        delete map;
    }
}

QTEST_MAIN(TestPerformance)
#include "test_performance.moc"
