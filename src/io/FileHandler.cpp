#include "io/FileHandler.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace IO {

Structure::String FileHandler::readText(const Structure::String& path) {
    // 使用二进制模式打开以确保跨平台一致性
    // C++20: 使用 char8_t* 构造 path 以支持 UTF-8
    std::filesystem::path filePath(reinterpret_cast<const char8_t*>(path.c_str()));
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path.c_str() << std::endl;
        return "";
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0) return "";

    char* buffer = new char[size + 1];
    file.read(buffer, size);
    buffer[size] = '\0';
    
    Structure::String content(buffer);
    delete[] buffer;
    file.close();
    return content;
}

bool FileHandler::writeText(const Structure::String& path, const Structure::String& content) {
    // 使用二进制模式写入以确保与 readText 一致，避免换行符转换
    std::filesystem::path filePath(reinterpret_cast<const char8_t*>(path.c_str()));
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path.c_str() << std::endl;
        return false;
    }

    file << content;
    file.close();
    return true;
}

Structure::ArrayList<unsigned char> FileHandler::readBinary(const Structure::String& path) {
    Structure::ArrayList<unsigned char> data;
    std::filesystem::path filePath(reinterpret_cast<const char8_t*>(path.c_str()));
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open binary file: " << path.c_str() << std::endl;
        return data;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > 0) {
        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
    }
    
    file.close();
    return data;
}

bool FileHandler::writeBinary(const Structure::String& path, const Structure::ArrayList<unsigned char>& data) {
    std::filesystem::path filePath(reinterpret_cast<const char8_t*>(path.c_str()));
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open binary file for writing: " << path.c_str() << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    file.close();
    return true;
}

}
