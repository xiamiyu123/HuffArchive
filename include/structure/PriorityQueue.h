#pragma once
#include "ArrayList.h"
#include <functional>
#include <utility> // 用于 std::swap

namespace Structure {

    // 默认使用 std::less，实现最小堆（如果 T 是基本类型）
    // 如果需要最大堆，可以传入 std::greater
    // 也可以传入自定义比较器
    template <typename T, typename Compare = std::less<T>>
    class PriorityQueue {
    private:
        ArrayList<T> m_data;
        Compare m_comp;

        void swim(int index) {
            while (index > 0) {
                int parent = (index - 1) / 2;
                // 如果当前元素比父元素"小"（优先级更高），则交换
                if (m_comp(m_data[index], m_data[parent])) {
                    std::swap(m_data[index], m_data[parent]);
                    index = parent;
                } else {
                    break;
                }
            }
        }

        void sink(int index) {
            int size = m_data.size();
            while (true) {
                int left = 2 * index + 1;
                int right = 2 * index + 2;
                int smallest = index;

                if (left < size && m_comp(m_data[left], m_data[smallest])) {
                    smallest = left;
                }
                if (right < size && m_comp(m_data[right], m_data[smallest])) {
                    smallest = right;
                }

                if (smallest != index) {
                    std::swap(m_data[index], m_data[smallest]);
                    index = smallest;
                } else {
                    break;
                }
            }
        }

    public:
        PriorityQueue() {}
        
        // 传入自定义比较器的构造函数
        PriorityQueue(Compare comp) : m_comp(comp) {}

        // O(n) 建堆构造函数
        PriorityQueue(const ArrayList<T>& items, Compare comp = Compare()) : m_data(items), m_comp(comp) {
            // 从最后一个非叶子节点开始下沉
            for (int i = (m_data.size() / 2) - 1; i >= 0; --i) {
                sink(i);
            }
        }

        void push(const T& value) {
            m_data.add(value);
            swim(m_data.size() - 1);
        }

        void pop() {
            if (empty()) return;
            // 将根节点与最后一个节点交换
            std::swap(m_data[0], m_data[m_data.size() - 1]);
            // 移除最后一个节点（原根节点）
            m_data.remove(m_data.size() - 1);
            // 恢复堆性质
            if (!empty()) {
                sink(0);
            }
        }

        T& top() {
            if (empty()) throw std::out_of_range("PriorityQueue is empty");
            return m_data[0];
        }

        const T& top() const {
            if (empty()) throw std::out_of_range("PriorityQueue is empty");
            return m_data[0];
        }

        bool empty() const {
            return m_data.empty();
        }

        int size() const {
            return m_data.size();
        }

        void clear() {
            m_data.clear();
        }

        void swap(PriorityQueue& other) noexcept {
            m_data.swap(other.m_data);
            std::swap(m_comp, other.m_comp);
        }
    };
}
