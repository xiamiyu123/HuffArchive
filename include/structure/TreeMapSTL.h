#pragma once

#include "MapHuff.h"
#include <map>
#include <functional>

namespace Structure {

template <typename K, typename V>
class TreeMapSTL : public MapHuff<K, V> {
private:
    std::map<K, V> m_map;

public:
    TreeMapSTL() {}
    ~TreeMapSTL() override {}

    // 插入键值对
    void put(const K& key, const V& value) override {
        m_map[key] = value;
    }

    // 获取值（返回指针，若不存在返回 nullptr）
    V* get(const K& key) override {
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    // 检查是否包含键
    bool contains(const K& key) const override {
        return m_map.find(key) != m_map.end();
    }

    // 获取大小
    int size() const override {
        return static_cast<int>(m_map.size());
    }

    // 检查是否为空
    bool isEmpty() const override {
        return m_map.empty();
    }

    // 遍历所有元素
    void traverse(std::function<void(const K&, const V&)> callback) const override {
        for (const auto& pair : m_map) {
            callback(pair.first, pair.second);
        }
    }
};

}
