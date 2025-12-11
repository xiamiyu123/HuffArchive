#include "model/DataModel.h"

namespace Model {

DataModel::DataModel() {
}

DataModel::~DataModel() {
    m_files.clear(); // ArrayList destructor handles memory, but clear is good practice if it resets size
}

void DataModel::addFile(const FileRecord& file) {
    m_files.add(file);
}

void DataModel::removeFile(int index) {
    m_files.removeAt(index);
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
