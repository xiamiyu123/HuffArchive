#pragma once

#include "ArrayList.h"

namespace Structure {

template <typename Key, typename Value>
class LLRBTree {
private:
    static const bool RED = true;
    static const bool BLACK = false;

    struct Node {
        Key key;
        Value val;
        Node* left;
        Node* right;
        bool color; // 指向该节点的链接颜色

        Node(Key k, Value v, bool col)
            : key(k), val(v), left(nullptr), right(nullptr), color(col) {}
    };

    Node* root;
    int m_size;

    // 辅助函数：判断节点颜色
    bool isRed(Node* x) const {
        if (x == nullptr) return false;
        return x->color == RED;
    }

    // 左旋转
    Node* rotateLeft(Node* h) {
        Node* x = h->right;
        h->right = x->left;
        x->left = h;
        x->color = h->color;
        h->color = RED;
        return x;
    }

    // 右旋转
    Node* rotateRight(Node* h) {
        Node* x = h->left;
        h->left = x->right;
        x->right = h;
        x->color = h->color;
        h->color = RED;
        return x;
    }

    // 颜色翻转
    void flipColors(Node* h) {
        h->color = !h->color;
        h->left->color = !h->left->color;
        h->right->color = !h->right->color;
    }

    // 插入节点的递归实现
    Node* put(Node* h, Key key, Value val) {
        if (h == nullptr) {
            m_size++;
            return new Node(key, val, RED);
        }

        if (key < h->key) h->left = put(h->left, key, val);
        else if (key > h->key) h->right = put(h->right, key, val);
        else h->val = val;

        // 修复红黑树性质
        if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
        if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
        if (isRed(h->left) && isRed(h->right)) flipColors(h);

        return h;
    }

    // 查找节点的辅助函数
    Value* get(Node* x, Key key) {
        while (x != nullptr) {
            if (key < x->key) x = x->left;
            else if (key > x->key) x = x->right;
            else return &(x->val);
        }
        return nullptr;
    }
    
    // 递归销毁树
    void destroy(Node* x) {
        if (x == nullptr) return;
        destroy(x->left);
        destroy(x->right);
        delete x;
    }

    // 中序遍历获取键
    void collectKeys(Node* x, ArrayList<Key>& list) const {
        if (x == nullptr) return;
        collectKeys(x->left, list);
        list.add(x->key);
        collectKeys(x->right, list);
    }

    // 遍历辅助函数
    void traverse(Node* x, std::function<void(const Key&, const Value&)> callback) const {
        if (x == nullptr) return;
        traverse(x->left, callback);
        callback(x->key, x->val);
        traverse(x->right, callback);
    }

public:
    LLRBTree() : root(nullptr), m_size(0) {}
    
    ~LLRBTree() {
        destroy(root);
    }

    // 获取大小
    int size() const {
        return m_size;
    }

    // 插入键值对
    void put(Key key, Value val) {
        root = put(root, key, val);
        root->color = BLACK;
    }

    // 获取值（返回指针，若不存在返回 nullptr）
    Value* get(Key key) {
        return get(root, key);
    }
    
    // 检查是否包含键
    bool contains(Key key) const {
        Node* x = root;
        while (x != nullptr) {
            if (key < x->key) x = x->left;
            else if (key > x->key) x = x->right;
            else return true;
        }
        return false;
    }
    
    // 检查树是否为空
    bool isEmpty() const {
        return root == nullptr;
    }

    // 遍历
    void traverse(std::function<void(const Key&, const Value&)> callback) const {
        traverse(root, callback);
    }

    // 获取所有键（有序）
    ArrayList<Key> keys() const {
        ArrayList<Key> list;
        collectKeys(root, list);
        return list;
    }
};

}
