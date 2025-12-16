#include <QtTest>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <string>
#include "structure/MapFactory.h"
#include "structure/String.h"

using namespace Structure;

class TestPerformance : public QObject
{
    Q_OBJECT

private slots:
    void comparePerformance();
};

struct PerformanceResult {
    std::string name;
    double insertTime;
    double getTime;
    double containsTime;
};

void TestPerformance::comparePerformance()
{
    const int DATA_SIZE = 100000; // Data size
    std::cout << "\n========================================" << std::endl;
    std::cout << "Performance Comparison (Data Size: " << DATA_SIZE << ")" << std::endl;
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

    std::vector<std::pair<std::string, MapType>> mapTypes = {
        {"HashMap", MapType::HASH_MAP},
        {"TreeMap (LLRB)", MapType::TREE_MAP},
        {"TreeMap (STL)", MapType::TREE_MAP_STL}
    };

    std::vector<PerformanceResult> results;

    for (const auto& pair : mapTypes) {
        MapHuff<int, int>* map = MapFactory<int, int>::createMap(pair.second);
        PerformanceResult result;
        result.name = pair.first;

        // Test Insert
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->put(keys[i], values[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        result.insertTime = std::chrono::duration<double, std::milli>(end - start).count();

        // Test Get
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->get(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        result.getTime = std::chrono::duration<double, std::milli>(end - start).count();

        // Test Contains
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->contains(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        result.containsTime = std::chrono::duration<double, std::milli>(end - start).count();

        results.push_back(result);
        delete map;
    }

    // Print Table
    std::cout << std::left << std::setw(20) << "Map Type" 
              << std::setw(15) << "Insert (ms)" 
              << std::setw(15) << "Get (ms)" 
              << std::setw(15) << "Contains (ms)" << std::endl;
    std::cout << std::string(65, '-') << std::endl;

    for (const auto& res : results) {
        std::cout << std::left << std::setw(20) << res.name 
                  << std::setw(15) << res.insertTime 
                  << std::setw(15) << res.getTime 
                  << std::setw(15) << res.containsTime << std::endl;
    }
    std::cout << std::string(65, '-') << std::endl;
}

QTEST_MAIN(TestPerformance)
#include "test_performance.moc"
