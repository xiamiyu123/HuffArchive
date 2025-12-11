#pragma once
#include <functional>
#include <cstddef>

namespace Structure {

    template <typename K, typename V>
    class HashMap {
    public:
        struct Entry {
            K first;
            V second;
            Entry* next;
            
            Entry(const K& k, const V& v) : first(k), second(v), next(nullptr) {}
        };

    private:
        Entry** m_buckets;
        int m_size;
        int m_capacity;
        static constexpr double LOAD_FACTOR = 0.75;

        int hash(const K& key) const {
            return std::hash<K>{}(key) % m_capacity;
        }

        void rehash(int newCapacity) {
            Entry** newBuckets = new Entry*[newCapacity]();
            
            for (int i = 0; i < m_capacity; ++i) {
                Entry* current = m_buckets[i];
                while (current) {
                    Entry* next = current->next;
                    
                    // Re-hash
                    int newIndex = std::hash<K>{}(current->first) % newCapacity;
                    current->next = newBuckets[newIndex];
                    newBuckets[newIndex] = current;
                    
                    current = next;
                }
            }
            
            delete[] m_buckets;
            m_buckets = newBuckets;
            m_capacity = newCapacity;
        }

    public:
        HashMap(int initialCapacity = 16) : m_size(0), m_capacity(initialCapacity) {
            m_buckets = new Entry*[m_capacity]();
        }

        ~HashMap() {
            clear();
            delete[] m_buckets;
        }

        // 拷贝构造函数
        HashMap(const HashMap& other) : m_size(0), m_capacity(other.m_capacity) {
            m_buckets = new Entry*[m_capacity]();
            for (auto it = other.begin(); it != other.end(); ++it) {
                put(it->first, it->second);
            }
        }

        // 赋值运算符
        HashMap& operator=(const HashMap& other) {
            if (this != &other) {
                clear();
                delete[] m_buckets;
                
                m_capacity = other.m_capacity;
                m_buckets = new Entry*[m_capacity]();
                for (auto it = other.begin(); it != other.end(); ++it) {
                    put(it->first, it->second);
                }
            }
            return *this;
        }

        // 移动构造函数
        HashMap(HashMap&& other) noexcept 
            : m_buckets(other.m_buckets), m_size(other.m_size), m_capacity(other.m_capacity) {
            other.m_buckets = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        // 移动赋值运算符
        HashMap& operator=(HashMap&& other) noexcept {
            if (this != &other) {
                clear();
                delete[] m_buckets;
                
                m_buckets = other.m_buckets;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                
                other.m_buckets = nullptr;
                other.m_size = 0;
                other.m_capacity = 0;
            }
            return *this;
        }

        void put(const K& key, const V& value) {
            if (m_size >= m_capacity * LOAD_FACTOR) {
                rehash(m_capacity * 2);
            }

            int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    current->second = value; // Update
                    return;
                }
                current = current->next;
            }

            // 插入新元素
            Entry* newEntry = new Entry(key, value);
            newEntry->next = m_buckets[index];
            m_buckets[index] = newEntry;
            m_size++;
        }

        bool contains(const K& key) const {
            int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    return true;
                }
                current = current->next;
            }
            return false;
        }

        bool remove(const K& key) {
            int index = hash(key);
            Entry* current = m_buckets[index];
            Entry* prev = nullptr;

            while (current) {
                if (current->first == key) {
                    if (prev) {
                        prev->next = current->next;
                    } else {
                        m_buckets[index] = current->next;
                    }
                    delete current;
                    m_size--;
                    return true;
                }
                prev = current;
                current = current->next;
            }
            return false;
        }

        V& at(const K& key) {
            int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    return current->second;
                }
                current = current->next;
            }
            throw std::out_of_range("Key not found in HashMap");
        }

        const V& at(const K& key) const {
            int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    return current->second;
                }
                current = current->next;
            }
            throw std::out_of_range("Key not found in HashMap");
        }

        void swap(HashMap& other) noexcept {
            std::swap(m_buckets, other.m_buckets);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
        }

        V& operator[](const K& key) {
            int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    return current->second;
                }
                current = current->next;
            }

            // 未发现，插入默认值
            if (m_size >= m_capacity * LOAD_FACTOR) {
                rehash(m_capacity * 2);
                index = hash(key); // 重新计算索引
            }
            
            Entry* newEntry = new Entry(key, V());
            newEntry->next = m_buckets[index];
            m_buckets[index] = newEntry;
            m_size++;
            return newEntry->second;
        }
        
        class Iterator {
            Entry** m_buckets;
            int m_capacity;
            int m_bucketIndex;
            Entry* m_currentEntry;

        public:
            Iterator(Entry** buckets, int capacity, int index, Entry* entry) 
                : m_buckets(buckets), m_capacity(capacity), m_bucketIndex(index), m_currentEntry(entry) {
                if (!m_currentEntry && m_bucketIndex < m_capacity) {
                    advance();
                }
            }

            void advance() {
                if (m_currentEntry) {
                    m_currentEntry = m_currentEntry->next;
                }
                
                while (!m_currentEntry) {
                    m_bucketIndex++;
                    if (m_bucketIndex >= m_capacity) break;
                    m_currentEntry = m_buckets[m_bucketIndex];
                }
            }

            Iterator& operator++() {
                advance();
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return m_bucketIndex != other.m_bucketIndex || m_currentEntry != other.m_currentEntry;
            }
            
            bool operator==(const Iterator& other) const {
                return !(*this != other);
            }

            Entry& operator*() {
                return *m_currentEntry;
            }

            Entry* operator->() {
                return m_currentEntry;
            }
        };
        
        // 常量迭代器
        class ConstIterator {
            Entry** m_buckets;
            int m_capacity;
            int m_bucketIndex;
            Entry* m_currentEntry;

        public:
            ConstIterator(Entry** buckets, int capacity, int index, Entry* entry) 
                : m_buckets(buckets), m_capacity(capacity), m_bucketIndex(index), m_currentEntry(entry) {
                if (!m_currentEntry && m_bucketIndex < m_capacity) {
                    advance();
                }
            }

            void advance() {
                if (m_currentEntry) {
                    m_currentEntry = m_currentEntry->next;
                }
                
                while (!m_currentEntry) {
                    m_bucketIndex++;
                    if (m_bucketIndex >= m_capacity) break;
                    m_currentEntry = m_buckets[m_bucketIndex];
                }
            }

            ConstIterator& operator++() {
                advance();
                return *this;
            }

            bool operator!=(const ConstIterator& other) const {
                return m_bucketIndex != other.m_bucketIndex || m_currentEntry != other.m_currentEntry;
            }
            
            bool operator==(const ConstIterator& other) const {
                return !(*this != other);
            }

            const Entry& operator*() const {
                return *m_currentEntry;
            }

            const Entry* operator->() const {
                return m_currentEntry;
            }
        };

        Iterator begin() {
            return Iterator(m_buckets, m_capacity, -1, nullptr);
        }

        Iterator end() {
            return Iterator(m_buckets, m_capacity, m_capacity, nullptr);
        }
        
        ConstIterator begin() const {
            return ConstIterator(m_buckets, m_capacity, -1, nullptr);
        }

        ConstIterator end() const {
            return ConstIterator(m_buckets, m_capacity, m_capacity, nullptr);
        }
        
        Iterator find(const K& key) {
             int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    return Iterator(m_buckets, m_capacity, index, current);
                }
                current = current->next;
            }
            return end();
        }
        
        ConstIterator find(const K& key) const {
             int index = hash(key);
            Entry* current = m_buckets[index];
            while (current) {
                if (current->first == key) {
                    return ConstIterator(m_buckets, m_capacity, index, current);
                }
                current = current->next;
            }
            return end();
        }

        void clear() {
            for (int i = 0; i < m_capacity; ++i) {
                Entry* current = m_buckets[i];
                while (current) {
                    Entry* next = current->next;
                    delete current;
                    current = next;
                }
                m_buckets[i] = nullptr;
            }
            m_size = 0;
        }
        
        int size() const { return m_size; }
        bool empty() const { return m_size == 0; }
    };
}
