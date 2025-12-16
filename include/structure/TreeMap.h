#pragma once

#include "MapHuff.h"
#include "LLRBTree.h"
#include "ArrayList.h"

namespace Structure {

template <typename Key, typename Value>
class TreeMap : public MapHuff<Key, Value> {
private:
    LLRBTree<Key, Value> tree;

public:
    TreeMap() {}
    ~TreeMap() override {}

    // 插入键值对
    void put(const Key& key, const Value& val) override {
        tree.put(key, val);
    }

    // 获取值指针
    Value* get(const Key& key) override {
        return tree.get(key);
    }

    // 检查是否包含键
    bool contains(const Key& key) const override {
        return tree.contains(key);
    }

    // 获取大小
    int size() const override {
        return tree.size();
    }

    // 检查是否为空
    bool isEmpty() const override {
        return tree.isEmpty();
    }

    // 遍历
    void traverse(std::function<void(const Key&, const Value&)> callback) const override {
        tree.traverse(callback);
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
