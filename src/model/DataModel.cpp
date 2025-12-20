#include "model/DataModel.h"

namespace Model {

DataModel::DataModel() {
}

DataModel::~DataModel() {
    m_files.clear(); // ArrayList 析构函数处理内存，但如果 clear 能重置大小，则是良好的实践
}

void DataModel::addFile(const FileRecord& file) {
    m_files.add(file);
}

void DataModel::removeFile(int index) {
    m_files.remove(index);
}

void DataModel::clear() {
    m_files.clear();
}

FileRecord& DataModel::getFile(int index) {
    return m_files[index];
}

const FileRecord& DataModel::getFile(int index) const {
    return m_files[index];
}

int DataModel::getFileCount() const {
    return m_files.size();
}

const Structure::ArrayList<FileRecord>& DataModel::getFiles() const {
    return m_files;
}

}
