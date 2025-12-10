#include "structure/String.h"
#include <cstring>
#include <algorithm>

namespace Structure {

String::String() : m_data(new char[1]), m_length(0) {
    m_data[0] = '\0';
}

String::String(const char* str) {
    if (str) {
        m_length = std::strlen(str);
        m_data = new char[m_length + 1];
        std::strcpy(m_data, str);
    } else {
        m_length = 0;
        m_data = new char[1];
        m_data[0] = '\0';
    }
}

String::String(const String& other) {
    m_length = other.m_length;
    m_data = new char[m_length + 1];
    std::strcpy(m_data, other.m_data);
}

String::String(String&& other) noexcept : m_data(other.m_data), m_length(other.m_length) {
    other.m_data = nullptr;
    other.m_length = 0;
}

String::~String() {
    if (m_data) {
        delete[] m_data;
    }
}

String& String::operator=(const String& other) {
    if (this != &other) {
        delete[] m_data;
        m_length = other.m_length;
        m_data = new char[m_length + 1];
        std::strcpy(m_data, other.m_data);
    }
    return *this;
}

String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        delete[] m_data;
        m_data = other.m_data;
        m_length = other.m_length;
        other.m_data = nullptr;
        other.m_length = 0;
    }
    return *this;
}

String& String::operator=(const char* str) {
    delete[] m_data;
    if (str) {
        m_length = std::strlen(str);
        m_data = new char[m_length + 1];
        std::strcpy(m_data, str);
    } else {
        m_length = 0;
        m_data = new char[1];
        m_data[0] = '\0';
    }
    return *this;
}

int String::length() const { return m_length; }
int String::size() const { return m_length; }
bool String::empty() const { return m_length == 0; }
const char* String::c_str() const { return m_data; }
char* String::data() { return m_data; }

char& String::operator[](int index) { return m_data[index]; }
const char& String::operator[](int index) const { return m_data[index]; }

String String::operator+(const String& other) const {
    String result;
    delete[] result.m_data;
    result.m_length = m_length + other.m_length;
    result.m_data = new char[result.m_length + 1];
    std::strcpy(result.m_data, m_data);
    std::strcat(result.m_data, other.m_data);
    return result;
}

String& String::operator+=(const String& other) {
    char* newData = new char[m_length + other.m_length + 1];
    std::strcpy(newData, m_data);
    std::strcat(newData, other.m_data);
    delete[] m_data;
    m_data = newData;
    m_length += other.m_length;
    return *this;
}

String String::operator+(const char* other) const {
    String result;
    delete[] result.m_data;
    int otherLen = other ? std::strlen(other) : 0;
    result.m_length = m_length + otherLen;
    result.m_data = new char[result.m_length + 1];
    std::strcpy(result.m_data, m_data);
    if (other) std::strcat(result.m_data, other);
    return result;
}

String& String::operator+=(const char* other) {
    if (!other) return *this;
    int otherLen = std::strlen(other);
    char* newData = new char[m_length + otherLen + 1];
    std::strcpy(newData, m_data);
    std::strcat(newData, other);
    delete[] m_data;
    m_data = newData;
    m_length += otherLen;
    return *this;
}

String& String::operator+=(char c) {
    char* newData = new char[m_length + 2];
    std::strcpy(newData, m_data);
    newData[m_length] = c;
    newData[m_length + 1] = '\0';
    delete[] m_data;
    m_data = newData;
    m_length++;
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
