#pragma once

#include "structure/String.h"

namespace Model {

struct FileRecord {
    // TODO: Define file record fields
    Structure::String fileName;
    long originalSize;
    long compressedSize;
    // ...
};

}
