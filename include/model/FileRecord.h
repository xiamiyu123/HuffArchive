#pragma once

#include <string>

namespace Model {

struct FileRecord {
    // TODO: Define file record fields
    std::string fileName;
    long originalSize;
    long compressedSize;
    // ...
};

}
