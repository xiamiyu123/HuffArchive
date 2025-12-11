#include "structure/String.h"
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace Structure {

String::String() : m_data(new char[1]), m_length(0), m_capacity(0) {
    m_data[0] = '\0';
}

String::String(const char* str) {
    if (str) {
        m_length = std::strlen(str);
        m_capacity = m_length;
        m_data = new char[m_capacity + 1];
        std::strcpy(m_data, str);
    } else {
        m_length = 0;
        m_capacity = 0;
        m_data = new char[1];
        m_data[0] = '\0';
    }
}

String::String(const String& other) {
    m_length = other.m_length;
    m_capacity = other.m_length;  // 拷贝时只分配必要的空间
    m_data = new char[m_capacity + 1];
    std::strcpy(m_data, other.m_data);
}

String::String(String&& other) noexcept 
    : m_data(other.m_data), m_length(other.m_length), m_capacity(other.m_capacity) {
    other.m_data = nullptr;
    other.m_length = 0;
    other.m_capacity = 0;
}

String::~String() {
    if (m_data) {
        delete[] m_data;
    }
}

String& String::operator=(const String& other) {
    if (this != &other) {
        if (other.m_length > m_capacity) {
            delete[] m_data;
            m_capacity = other.m_length;
            m_data = new char[m_capacity + 1];
        }
        m_length = other.m_length;
        std::strcpy(m_data, other.m_data);
    }
    return *this;
}

String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        delete[] m_data;
        m_data = other.m_data;
        m_length = other.m_length;
        m_capacity = other.m_capacity;
        other.m_data = nullptr;
        other.m_length = 0;
        other.m_capacity = 0;
    }
    return *this;
}

String& String::operator=(const char* str) {
    if (str) {
        int newLength = std::strlen(str);
        if (newLength > m_capacity) {
            delete[] m_data;
            m_capacity = newLength;
            m_data = new char[m_capacity + 1];
        }
        m_length = newLength;
        std::strcpy(m_data, str);
    } else {
        m_length = 0;
        m_data[0] = '\0';
    }
    return *this;
}

int String::length() const { return m_length; }
int String::size() const { return m_length; }
bool String::empty() const { return m_length == 0; }
const char* String::c_str() const { return m_data; }
char* String::data() { return m_data; }
int String::capacity() const { return m_capacity; }

void String::reserve(int newCapacity) {
    if (newCapacity > m_capacity) {
        char* newData = new char[newCapacity + 1];
        if (m_data) {
            std::strcpy(newData, m_data);
            delete[] m_data;
        } else {
            newData[0] = '\0';
        }
        m_data = newData;
        m_capacity = newCapacity;
    }
}

char& String::operator[](int index) { return m_data[index]; }
const char& String::operator[](int index) const { return m_data[index]; }

String String::operator+(const String& other) const {
    String result;
    result.reserve(m_length + other.m_length);
    std::strcpy(result.m_data, m_data);
    std::strcat(result.m_data, other.m_data);
    result.m_length = m_length + other.m_length;
    return result;
}

String& String::operator+=(const String& other) {
    int newLength = m_length + other.m_length;
    if (newLength > m_capacity) {
        // 使用 2x 增长策略，均摊 O(1)
        int newCapacity = (m_capacity == 0) ? 16 : m_capacity * 2;
        while (newCapacity < newLength) {
            newCapacity *= 2;
        }
        reserve(newCapacity);
    }
    std::strcpy(m_data + m_length, other.m_data);
    m_length = newLength;
    return *this;
}

String String::operator+(const char* other) const {
    String result;
    int otherLen = other ? std::strlen(other) : 0;
    result.reserve(m_length + otherLen);
    std::strcpy(result.m_data, m_data);
    if (other) std::strcat(result.m_data, other);
    result.m_length = m_length + otherLen;
    return result;
}

String& String::operator+=(const char* other) {
    if (!other) return *this;
    int otherLen = std::strlen(other);
    int newLength = m_length + otherLen;
    if (newLength > m_capacity) {
        // 使用 2x 增长策略，均摊 O(1)
        int newCapacity = (m_capacity == 0) ? 16 : m_capacity * 2;
        while (newCapacity < newLength) {
            newCapacity *= 2;
        }
        reserve(newCapacity);
    }
    std::strcpy(m_data + m_length, other);
    m_length = newLength;
    return *this;
}

String& String::operator+=(char c) {
    int newLength = m_length + 1;
    if (newLength > m_capacity) {
        // 使用 2x 增长策略，均摊 O(1)
        int newCapacity = (m_capacity == 0) ? 16 : m_capacity * 2;
        reserve(newCapacity);
    }
    m_data[m_length] = c;
    m_data[newLength] = '\0';
    m_length = newLength;
    return *this;
}

bool String::operator==(const String& other) const {
    return std::strcmp(m_data, other.m_data) == 0;
}

bool String::operator!=(const String& other) const {
    return !(*this == other);
}

bool String::operator==(const char* other) const {
    if (!other) return false;
    return std::strcmp(m_data, other) == 0;
}

bool String::operator!=(const char* other) const {
    return !(*this == other);
}

bool String::operator<(const String& other) const {
    return std::strcmp(m_data, other.m_data) < 0;
}

String String::substr(int pos, int len) const {
    if (pos < 0 || pos >= m_length) return String();
    if (len == -1 || pos + len > m_length) len = m_length - pos;
    
    char* sub = new char[len + 1];
    std::strncpy(sub, m_data + pos, len);
    sub[len] = '\0';
    String result(sub);
    delete[] sub;
    return result;
}

int String::find(char c, int pos) const {
    for (int i = pos; i < m_length; ++i) {
        if (m_data[i] == c) return i;
    }
    return -1;
}

char* String::begin() { return m_data; }
char* String::end() { return m_data + m_length; }
const char* String::begin() const { return m_data; }
const char* String::end() const { return m_data + m_length; }

bool String::operator>(const String& other) const {
    return std::strcmp(m_data, other.m_data) > 0;
}

bool String::operator<=(const String& other) const {
    return !(*this > other);
}

bool String::operator>=(const String& other) const {
    return !(*this < other);
}

void String::clear() {
    m_length = 0;
    m_data[0] = '\0';
}

void String::swap(String& other) {
    std::swap(m_data, other.m_data);
    std::swap(m_length, other.m_length);
    std::swap(m_capacity, other.m_capacity);
}

char& String::at(int index) {
    if (index < 0 || index >= m_length) {
        throw std::out_of_range("String index out of range");
    }
    return m_data[index];
}

const char& String::at(int index) const {
    if (index < 0 || index >= m_length) {
        throw std::out_of_range("String index out of range");
    }
    return m_data[index];
}

void String::push_back(char c) {
    *this += c;
}

void String::pop_back() {
    if (m_length > 0) {
        m_length--;
        m_data[m_length] = '\0';
    }
}

char& String::front() {
    return m_data[0];
}

const char& String::front() const {
    return m_data[0];
}

char& String::back() {
    return m_data[m_length - 1];
}

const char& String::back() const {
    return m_data[m_length - 1];
}

std::ostream& operator<<(std::ostream& os, const String& str) {
    os << str.m_data;
    return os;
}

std::istream& operator>>(std::istream& is, String& str) {
    char buffer[4096];
    is >> buffer;
    str = buffer;
    return is;
}

}
