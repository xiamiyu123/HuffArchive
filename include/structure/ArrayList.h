#pragma once
#include <stdexcept>
#include <algorithm>

namespace Structure
{
    template <typename T>
    class ArrayList
    {
        private:
            T* m_data;
            int m_size;
            int m_capacity;
            static constexpr double GROWTH_FACTOR = 1.5;

            void reallocate(int newCapacity) {
                T* newData = new T[newCapacity];
                // 复制现有元素
                int elementsToCopy = (m_size < newCapacity) ? m_size : newCapacity;
                for (int i = 0; i < elementsToCopy; ++i) {
                    newData[i] = m_data[i];
                }
                delete[] m_data;
                m_data = newData;
                m_capacity = newCapacity;
            }

        public:
            ArrayList() : m_data(nullptr), m_size(0), m_capacity(0) {}
            
            ~ArrayList() {
                if (m_data) {
                    delete[] m_data;
                }
            }

            // 拷贝构造函数
            ArrayList(const ArrayList& other) : m_data(nullptr), m_size(0), m_capacity(0) {
                if (other.m_capacity > 0) {
                    reallocate(other.m_capacity);
                    m_size = other.m_size;
                    for (int i = 0; i < m_size; ++i) {
                        m_data[i] = other.m_data[i];
                    }
                }
            }

            // 移动构造函数
            ArrayList(ArrayList&& other) noexcept : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
                other.m_data = nullptr;
                other.m_size = 0;
                other.m_capacity = 0;
            }

            // 赋值运算符
            ArrayList& operator=(const ArrayList& other) {
                if (this != &other) {
                    if (other.m_size > m_capacity) {
                        reallocate(other.m_capacity);
                    }
                    m_size = other.m_size;
                    for (int i = 0; i < m_size; ++i) {
                        m_data[i] = other.m_data[i];
                    }
                }
                return *this;
            }

            // 移动赋值运算符
            ArrayList& operator=(ArrayList&& other) noexcept {
                if (this != &other) {
                    if (m_data) delete[] m_data;
                    
                    m_data = other.m_data;
                    m_size = other.m_size;
                    m_capacity = other.m_capacity;
                    
                    other.m_data = nullptr;
                    other.m_size = 0;
                    other.m_capacity = 0;
                }
                return *this;
            }

            void add(const T& item) {
                if (m_size == m_capacity) {
                    int newCapacity = (m_capacity == 0) ? 4 : static_cast<int>(m_capacity * GROWTH_FACTOR);
                    reallocate(newCapacity);
                }
                m_data[m_size++] = item;
            }
            
            void push_back(const T& item) {
                add(item);
            }

            void remove(int index) {
                if (index < 0 || index >= m_size) throw std::out_of_range("Index out of range");
                for (int i = index; i < m_size - 1; ++i) {
                    m_data[i] = m_data[i + 1];
                }
                m_size--;
            }

            T& get(int index) {
                if (index < 0 || index >= m_size) throw std::out_of_range("Index out of range");
                return m_data[index];
            }
            
            const T& get(int index) const {
                if (index < 0 || index >= m_size) throw std::out_of_range("Index out of range");
                return m_data[index];
            }

            T& operator[](int index) {
                return m_data[index];
            }

            const T& operator[](int index) const {
                return m_data[index];
            }

            int size() const { return m_size; }
            int capacity() const { return m_capacity; }

            void shrink_to_fit() {
                if (m_size < m_capacity) {
                    // 如果为空，释放内存
                    if (m_size == 0) {
                        delete[] m_data;
                        m_data = nullptr;
                        m_capacity = 0;
                    } else {
                        // 重新分配精确大小
                        T* newData = new T[m_size];
                        for (int i = 0; i < m_size; ++i) {
                            newData[i] = m_data[i]; // 假设 T 支持拷贝
                        }
                        delete[] m_data;
                        m_data = newData;
                        m_capacity = m_size;
                    }
                }
            }

            void insert(int index, const T& item) {
                if (index < 0 || index > m_size) throw std::out_of_range("Index out of range");
                
                if (m_size == m_capacity) {
                    int newCapacity = (m_capacity == 0) ? 4 : static_cast<int>(m_capacity * GROWTH_FACTOR);
                    reallocate(newCapacity);
                }

                // 移动元素
                for (int i = m_size; i > index; --i) {
                    m_data[i] = m_data[i - 1];
                }
                
                m_data[index] = item;
                m_size++;
            }

            void swap(ArrayList& other) noexcept {
                std::swap(m_data, other.m_data);
                std::swap(m_size, other.m_size);
                std::swap(m_capacity, other.m_capacity);
            }

            int indexOf(const T& item) const {
                for (int i = 0; i < m_size; ++i) {
                    if (m_data[i] == item) return i;
                }
                return -1;
            }

            bool contains(const T& item) const {
                return indexOf(item) != -1;
            }
            
            bool empty() const { return m_size == 0; }
            
            void clear() {
                m_size = 0;
            }
            
            void resize(int newSize) {
                if (newSize > m_capacity) {
                    reallocate(newSize);
                }
                // 默认构造新元素
                for (int i = m_size; i < newSize; ++i) {
                    m_data[i] = T();
                }
                m_size = newSize;
            }

            void reserve(int newCapacity) {
                if (newCapacity > m_capacity) {
                    reallocate(newCapacity);
                }
            }

            T* data() { return m_data; }
            const T* data() const { return m_data; }

            void pop_back() {
                if (m_size > 0) m_size--;
            }

            T& back() {
                if (m_size == 0) throw std::out_of_range("ArrayList is empty");
                return m_data[m_size - 1];
            }
            
            const T& back() const {
                if (m_size == 0) throw std::out_of_range("ArrayList is empty");
                return m_data[m_size - 1];
            }

            T& front() {
                if (m_size == 0) throw std::out_of_range("ArrayList is empty");
                return m_data[0];
            }

            const T& front() const {
                if (m_size == 0) throw std::out_of_range("ArrayList is empty");
                return m_data[0];
            }

            // 假装迭代器喵(?)以支持范围 for 循环和算法
            T* begin() { return m_data; }
            T* end() { return m_data + m_size; }
            const T* begin() const { return m_data; }
            const T* end() const { return m_data + m_size; }
    };

}
