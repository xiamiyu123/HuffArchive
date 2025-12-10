#pragma once

#include <string>
#include "structure/ArrayList.h"

namespace IO {

class FileHandler {
public:
    FileHandler() = default;
    ~FileHandler() = default;

    // 读取文本文件
    static std::string readText(const std::string& path);
    
    // 写入文本文件
    static bool writeText(const std::string& path, const std::string& content);

    // 读取二进制文件
    static Structure::ArrayList<unsigned char> readBinary(const std::string& path);

    // 写入二进制文件
    static bool writeBinary(const std::string& path, const Structure::ArrayList<unsigned char>& data);
};

}
