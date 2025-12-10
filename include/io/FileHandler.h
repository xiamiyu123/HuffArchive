#pragma once

#include "structure/String.h"
#include "structure/ArrayList.h"

namespace IO {

class FileHandler {
public:
    FileHandler() = default;
    ~FileHandler() = default;

    // 读取文本文件
    static Structure::String readText(const Structure::String& path);
    
    // 写入文本文件
    static bool writeText(const Structure::String& path, const Structure::String& content);

    // 读取二进制文件
    static Structure::ArrayList<unsigned char> readBinary(const Structure::String& path);

    // 写入二进制文件
    static bool writeBinary(const Structure::String& path, const Structure::ArrayList<unsigned char>& data);
};

}
