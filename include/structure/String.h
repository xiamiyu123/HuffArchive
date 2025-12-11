#pragma once
#include <iostream>
#include <cstring>

namespace Structure {

class String {
private:
    char* m_data;
    int m_length;
    int m_capacity;  // 新增：当前分配的容量（不含终止符）

public:
    String();
    String(const char* str);
    String(const String& other);
    String(String&& other) noexcept;
    ~String();

    String& operator=(const String& other);
    String& operator=(String&& other) noexcept;
    String& operator=(const char* str);

    int length() const;
    int size() const;
    bool empty() const;
    const char* c_str() const;
    char* data();
    int capacity() const;  // 新增：获取当前容量

    // 新增：预分配容量，避免频繁重新分配
    void reserve(int newCapacity);

    char& operator[](int index);
    const char& operator[](int index) const;

    String operator+(const String& other) const;
    String& operator+=(const String& other);
    String operator+(const char* other) const;
    String& operator+=(const char* other);
    String& operator+=(char c);

    bool operator==(const String& other) const;
    bool operator!=(const String& other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    bool operator<(const String& other) const;

    String substr(int pos, int len = -1) const;
    int find(char c, int pos = 0) const;
    
    char* begin();
    char* end();
    const char* begin() const;
    const char* end() const;

    friend std::ostream& operator<<(std::ostream& os, const String& str);
    friend std::istream& operator>>(std::istream& is, String& str);
};

// Helper for QTest
inline char* toString(const String& str) {
    int len = str.length();
    char* buffer = new char[len + 1];
    std::strcpy(buffer, str.c_str());
    return buffer;
}

}

// 特化 std::hash 以支持 Structure::String
namespace std {
    template <>
    struct hash<Structure::String> {
        size_t operator()(const Structure::String& str) const {
            // 使用 DJB2 哈希算法
            size_t hash = 5381;
            for (const char* c = str.c_str(); *c != '\0'; ++c) {
                hash = ((hash << 5) + hash) + static_cast<unsigned char>(*c);
            }
            return hash;
        }
    };
}

