#include <QtTest>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <string>
#include "structure/MapFactory.h"
#include "structure/String.h"
#include "structure/HuffmanTree.h"

using namespace Structure;

class TestPerformance : public QObject
{
    Q_OBJECT

private slots:
    void comparePerformance();
};

struct MapPerformanceResult {
    std::string name;
    double insertTime;
    double getTime;
    double containsTime;
};

struct HuffmanPerformanceResult {
    std::string name;
    double freqCountTime;
    double treeBuildTime;
};

void TestPerformance::comparePerformance()
{
    // --- 第一部分：Map 基础性能测试 ---
    const int DATA_SIZE = 1000000; // 数据量
    std::cout << "\n========================================" << std::endl;
    std::cout << "Map 基础性能对比 (数据量: " << DATA_SIZE << ")" << std::endl;
    std::cout << "========================================" << std::endl;

    // 准备随机数据
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

    std::vector<MapPerformanceResult> mapResults;

    for (const auto& pair : mapTypes) {
        MapHuff<int, int>* map = MapFactory<int, int>::createMap(pair.second);
        MapPerformanceResult result;
        result.name = pair.first;

        // 测试插入
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->put(keys[i], values[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        result.insertTime = std::chrono::duration<double, std::milli>(end - start).count();

        // 测试获取
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->get(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        result.getTime = std::chrono::duration<double, std::milli>(end - start).count();

        // 测试包含
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < DATA_SIZE; ++i) {
            map->contains(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        result.containsTime = std::chrono::duration<double, std::milli>(end - start).count();

        mapResults.push_back(result);
        delete map;
    }

    // 打印表格
    std::cout << std::left << std::setw(20) << "Map 类型" 
              << std::setw(15) << "插入 (ms)" 
              << std::setw(15) << "获取 (ms)" 
              << std::setw(15) << "查找 (ms)" << std::endl;
    std::cout << std::string(65, '-') << std::endl;

    for (const auto& res : mapResults) {
        std::cout << std::left << std::setw(20) << res.name 
                  << std::setw(15) << res.insertTime 
                  << std::setw(15) << res.getTime 
                  << std::setw(15) << res.containsTime << std::endl;
    }
    std::cout << std::string(65, '-') << std::endl;


    // --- 第二部分：哈夫曼树构建性能测试 ---
    const int FILE_SIZE = 1024 * 1024; // 1MB
    std::cout << "\n========================================" << std::endl;
    std::cout << "哈夫曼树构建性能对比 (数据量: 1MB 随机字节)" << std::endl;
    std::cout << "========================================" << std::endl;

    // 准备随机字节数据
    std::vector<unsigned char> fileData(FILE_SIZE);
    std::uniform_int_distribution<int> byteDist(0, 255);
    for (int i = 0; i < FILE_SIZE; ++i) {
        fileData[i] = static_cast<unsigned char>(byteDist(rng));
    }

    std::vector<HuffmanPerformanceResult> huffResults;

    for (const auto& pair : mapTypes) {
        HuffmanPerformanceResult result;
        result.name = pair.first;
        
        // 1. 统计频率
        MapHuff<unsigned char, int>* freqMap = MapFactory<unsigned char, int>::createMap(pair.second);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned char c : fileData) {
            int* count = freqMap->get(c);
            if (count) {
                (*count)++;
            } else {
                freqMap->put(c, 1);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        result.freqCountTime = std::chrono::duration<double, std::milli>(end - start).count();

        // 2. 构建哈夫曼树
        HuffmanTree huffTree;
        start = std::chrono::high_resolution_clock::now();
        huffTree.build(*freqMap);
        end = std::chrono::high_resolution_clock::now();
        result.treeBuildTime = std::chrono::duration<double, std::milli>(end - start).count();

        huffResults.push_back(result);
        delete freqMap;
    }

    // 打印表格
    std::cout << std::left << std::setw(20) << "Map 类型" 
              << std::setw(20) << "频率统计 (ms)" 
              << std::setw(20) << "建树 (ms)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    for (const auto& res : huffResults) {
        std::cout << std::left << std::setw(20) << res.name 
                  << std::setw(20) << res.freqCountTime 
                  << std::setw(20) << res.treeBuildTime << std::endl;
    }
    std::cout << std::string(60, '-') << std::endl;
}

QTEST_MAIN(TestPerformance)
#include "test_performance.moc"
