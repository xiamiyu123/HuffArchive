#pragma once
#include <functional>

namespace Structure {

template <typename K, typename V>
class MapHuff {
public:
    virtual ~MapHuff() {}
    
    // 插入键值对
    virtual void put(const K& key, const V& value) = 0;
    
    // 获取值（返回指针，若不存在返回 nullptr）
    virtual V* get(const K& key) = 0;
    
    // 检查是否包含键
    virtual bool contains(const K& key) const = 0;
    
    // 获取大小
    virtual int size() const = 0;
    
    // 检查是否为空
    virtual bool isEmpty() const = 0;
    
    // 遍历所有元素
    virtual void traverse(std::function<void(const K&, const V&)> callback) const = 0;
};

}
