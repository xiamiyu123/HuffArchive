#include "model/FileRecord.h"
#include <filesystem>
#include <cmath>
#include <cstdio>

namespace Model {

FileRecord::FileRecord() 
    : m_originalSize(0), m_compressedSize(0), m_offset(0),
      m_status(FileStatus::Pending), m_type(FileType::File) {
}

FileRecord::FileRecord(const Structure::String& filePath, FileType type)
    : m_filePath(filePath), m_originalSize(0), m_compressedSize(0), m_offset(0),
      m_status(FileStatus::Pending), m_type(type) {
    
    // 初始化时，默认相对路径为文件名
    // 后续在 DataModel 或 Command 中根据压缩根目录进行调整
    m_relativePath = getFileName();

    // 如果是普通文件，尝试自动获取大小
    if (m_type == FileType::File && !m_filePath.empty()) {
        try {
            std::filesystem::path p(m_filePath.c_str());
            if (std::filesystem::exists(p) && std::filesystem::is_regular_file(p)) {
                m_originalSize = static_cast<long long>(std::filesystem::file_size(p));
            }
        } catch (...) {
            m_originalSize = 0;
            m_message = "Failed to read file size";
        }
    }
}

Structure::String FileRecord::getFilePath() const {
    return m_filePath;
}

Structure::String FileRecord::getFileName() const {
    if (m_filePath.empty()) return "";
    try {
        std::filesystem::path p(m_filePath.c_str());
        // filename() 返回文件名+扩展名
        return Structure::String(p.filename().string().c_str());
    } catch (...) {
        return "";
    }
}

Structure::String FileRecord::getRelativePath() const {
    return m_relativePath;
}

long long FileRecord::getOriginalSize() const {
    return m_originalSize;
}

long long FileRecord::getCompressedSize() const {
    return m_compressedSize;
}

long long FileRecord::getOffset() const {
    return m_offset;
}

FileStatus FileRecord::getStatus() const {
    return m_status;
}

FileType FileRecord::getType() const {
    return m_type;
}

Structure::String FileRecord::getMessage() const {
    return m_message;
}

double FileRecord::getCompressionRatio() const {
    if (m_originalSize == 0) return 0.0;
    return static_cast<double>(m_compressedSize) / static_cast<double>(m_originalSize);
}

bool FileRecord::isDirectory() const {
    return m_type == FileType::Directory;
}

void FileRecord::setFilePath(const Structure::String& path) {
    m_filePath = path;
}

void FileRecord::setRelativePath(const Structure::String& path) {
    m_relativePath = path;
}

void FileRecord::setOriginalSize(long long size) {
    m_originalSize = size;
}

void FileRecord::setCompressedSize(long long size) {
    m_compressedSize = size;
}

void FileRecord::setOffset(long long offset) {
    m_offset = offset;
}

void FileRecord::setStatus(FileStatus status) {
    m_status = status;
}

void FileRecord::setMessage(const Structure::String& msg) {
    m_message = msg;
}

}
