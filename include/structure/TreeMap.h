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
        return const_cast<LLRBTree<Key, Value>&>(tree).contains(key);
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
        // 使用迭代器进行遍历
        auto& nonConstTree = const_cast<LLRBTree<Key, Value>&>(tree);
        for (auto it = nonConstTree.begin(); it != nonConstTree.end(); ++it) {
            auto entry = *it;
            callback(entry.first, entry.second);
        }
    }

    // 迭代器支持
    using Iterator = typename LLRBTree<Key, Value>::Iterator;
    
    Iterator begin() {
        return tree.begin();
    }
    
    Iterator end() {
        return tree.end();
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
