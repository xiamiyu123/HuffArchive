#pragma once

#include "ArrayList.h"
#include <stack>

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

    // 迭代器定义
    class Iterator {
    private:
        std::stack<Node*> stack;
        Node* current;

        void pushLeft(Node* x) {
            while (x != nullptr) {
                stack.push(x);
                x = x->left;
            }
        }

    public:
        Iterator(Node* root) : current(nullptr) {
            pushLeft(root);
            if (!stack.empty()) {
                current = stack.top();
                stack.pop();
            }
        }

        // 结束迭代器
        Iterator() : current(nullptr) {}

        bool operator!=(const Iterator& other) const {
            return current != other.current || !stack.empty() != !other.stack.empty(); 
            // 简化逻辑：只要 current 不同就是不同。对于 end()，current 是 nullptr 且 stack 为空。
            // 但为了严谨，如果两个迭代器都指向同一个非空节点，且 stack 状态不同（不可能发生于同一棵树的遍历），也算不同？
            // 通常只比较 current 即可，除非是 end iterator。
            // 修正：
            if (current == nullptr && other.current == nullptr) return false; // 都是 end
            return current != other.current;
        }

        // 解引用返回 Key-Value 对的引用是不行的，因为 Node 分开了 Key 和 Value。
        // 为了模仿 HashMap::Entry，我们可以返回一个临时对象或者 pair。
        // 但 HashMap::Iterator 返回 Entry&。
        // 这里我们定义一个 Entry 结构体或者直接返回 Node& (但不安全)。
        // 让我们在 LLRBTree 中定义一个 Entry 结构体用于迭代器返回。
        
        struct Entry {
            const Key& first;
            Value& second;
            Entry(const Key& k, Value& v) : first(k), second(v) {}
        };

        Entry operator*() {
            return Entry(current->key, current->val);
        }

        Iterator& operator++() {
            if (current == nullptr) return *this;

            pushLeft(current->right);
            
            if (!stack.empty()) {
                current = stack.top();
                stack.pop();
            } else {
                current = nullptr;
            }
            return *this;
        }
    };

    Iterator begin() {
        return Iterator(root);
    }

    Iterator end() {
        return Iterator();
    }
};

}
