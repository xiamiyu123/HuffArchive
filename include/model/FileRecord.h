#pragma once

#include "structure/String.h"

namespace Model {

// 文件状态
enum class FileStatus {
    Pending,    // 等待中
    Processing, // 处理中
    Completed,  // 完成
    Error       // 出错
};

// 文件类型
enum class FileType {
    File,       // 普通文件
    Directory   // 文件夹
};

class FileRecord {
public:
    FileRecord();
    // 构造函数：传入绝对路径和类型
    FileRecord(const Structure::String& filePath, FileType type = FileType::File);
    ~FileRecord() = default;

    // Getters
    Structure::String getFilePath() const;      // 获取绝对路径
    Structure::String getFileName() const;      // 获取文件名（带扩展名）
    Structure::String getRelativePath() const;  // 获取相对路径（用于压缩包内存储结构）
    
    long long getOriginalSize() const;          // 原始大小 (字节)
    long long getCompressedSize() const;        // 压缩后大小 (字节)
    long long getOffset() const;                // 在压缩包中的字节偏移量
    
    FileStatus getStatus() const;
    FileType getType() const;
    Structure::String getMessage() const;       // 获取错误或状态信息

    // Helpers
    double getCompressionRatio() const;         // 获取压缩率 (0.0 - 1.0)
    bool isDirectory() const;

    // Setters
    void setFilePath(const Structure::String& path);
    void setRelativePath(const Structure::String& path);
    void setOriginalSize(long long size);
    void setCompressedSize(long long size);
    void setOffset(long long offset);
    void setStatus(FileStatus status);
    void setMessage(const Structure::String& msg);

private:
    Structure::String m_filePath;     // 绝对路径 (例如: D:/Docs/data.txt)
    Structure::String m_relativePath; // 相对路径 (例如: Docs/data.txt，用于解压还原)
    
    long long m_originalSize;
    long long m_compressedSize;
    long long m_offset;               // 关键字段：在压缩包中的起始位置
    
    FileStatus m_status;
    FileType m_type;
    Structure::String m_message;
};

}
