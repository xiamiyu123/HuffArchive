#pragma once

#include "structure/ArrayList.h"
#include "model/FileRecord.h"

namespace Model {

class DataModel {
public:
    DataModel();
    ~DataModel();

    // 文件管理
    void addFile(const FileRecord& file);
    void removeFile(int index);
    void clear();
    
    FileRecord& getFile(int index);
    const FileRecord& getFile(int index) const;
    int getFileCount() const;
    
    const Structure::ArrayList<FileRecord>& getFiles() const;

private:
    Structure::ArrayList<FileRecord> m_files;
};

}
