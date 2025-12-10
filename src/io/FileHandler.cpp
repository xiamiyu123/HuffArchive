#include "io/FileHandler.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace IO {

std::string FileHandler::readText(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    return content;
}

bool FileHandler::writeText(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }

    file << content;
    file.close();
    return true;
}

Structure::ArrayList<unsigned char> FileHandler::readBinary(const std::string& path) {
    Structure::ArrayList<unsigned char> data;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open binary file: " << path << std::endl;
        return data;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > 0) {
        // 使用 std::vector 作为临时缓冲区，因为 ArrayList 可能没有 resize/data 方法直接暴露给 read
        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size)) {
            for (char c : buffer) {
                data.add(static_cast<unsigned char>(c));
            }
        }
    }
    
    file.close();
    return data;
}

bool FileHandler::writeBinary(const std::string& path, const Structure::ArrayList<unsigned char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open binary file for writing: " << path << std::endl;
        return false;
    }

    // ArrayList 应该支持迭代器或者索引访问
    // 为了效率，如果 ArrayList 有 data() 方法最好，如果没有就循环写入
    for (const auto& byte : data) {
        file.put(static_cast<char>(byte));
    }

    file.close();
    return true;
}

}
