#pragma once

#include "LLRBTree.h"
#include "ArrayList.h"

namespace Structure {

template <typename Key, typename Value>
class TreeMap {
private:
    LLRBTree<Key, Value> tree;

public:
    TreeMap() {}
    ~TreeMap() {}

    // 插入键值对
    void put(const Key& key, const Value& val) {
        tree.put(key, val);
    }

    // 获取值指针
    Value* get(const Key& key) {
        return tree.get(key);
    }

    // 检查是否包含键
    bool contains(const Key& key) {
        return tree.contains(key);
    }

    // 获取大小
    int size() const {
        return tree.size();
    }

    // 检查是否为空
    bool isEmpty() const {
        return tree.isEmpty();
    }

    // 获取所有键（有序）
    ArrayList<Key> keys() const {
        return tree.keys();
    }

    // 下标运算符
    Value& operator[](const Key& key) {
        Value* val = tree.get(key);
        if (val == nullptr) {
            // 如果不存在，插入默认值
            tree.put(key, Value());
            val = tree.get(key);
        }
        return *val;
    }
};

}
