#include "command/CompressMultipleSourcesCommand.h"
#include "command/CompressCommand.h"
#include <filesystem>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;

namespace Command {

CompressMultipleSourcesCommand::CompressMultipleSourcesCommand(
    const Structure::ArrayList<Structure::String>& sourcePaths,
    const Structure::String& outputPath)
    : m_sourcePaths(sourcePaths), m_outputPath(outputPath) {
}

CompressMultipleSourcesCommand::~CompressMultipleSourcesCommand() {
}

bool CompressMultipleSourcesCommand::execute() {
    try {
        // 1. 验证源路径列表
        if (m_sourcePaths.size() == 0) {
            m_errorMessage = Structure::String("No source paths provided");
            return false;
        }

        // 2. 清空模型
        m_model.clear();

        // 3. 收集所有文件
        collectAllFiles();

        // 4. 检查是否有文件
        if (m_model.getFileCount() == 0) {
            m_errorMessage = Structure::String("No files to compress");
            return false;
        }

        // 5. 计算相对路径
        computeRelativePaths();

        // 6. 调用底层压缩命令
        CompressCommand compressCmd(&m_model, m_outputPath);
        if (m_progressCallback) {
            compressCmd.setProgressCallback(m_progressCallback);
        }
        if (m_checkCancelCallback) {
            compressCmd.setCheckCancelCallback(m_checkCancelCallback);
        }
        compressCmd.execute();
        
        if (m_checkCancelCallback && m_checkCancelCallback()) {
            m_errorMessage = Structure::String("Operation cancelled by user");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_errorMessage = Structure::String(e.what());
        return false;
    }
}

Structure::String CompressMultipleSourcesCommand::getErrorMessage() const {
    return m_errorMessage;
}

const Model::DataModel& CompressMultipleSourcesCommand::getModel() const {
    return m_model;
}

void CompressMultipleSourcesCommand::collectAllFiles() {
    for (int i = 0; i < m_sourcePaths.size(); ++i) {
        const Structure::String& sourceStr = m_sourcePaths[i];
        fs::path sourcePath(reinterpret_cast<const char8_t*>(sourceStr.c_str()));

        if (!fs::exists(sourcePath)) {
            // 跳过不存在的路径（可选：记录警告）
            continue;
        }

        if (fs::is_regular_file(sourcePath)) {
            // 添加单个文件
            m_model.addFile(Model::FileRecord(sourceStr, Model::FileType::File));
        } else if (fs::is_directory(sourcePath)) {
            // 递归添加目录中的所有文件
            try {
                for (const auto& entry : fs::recursive_directory_iterator(sourcePath)) {
                    if (fs::is_regular_file(entry)) {
                        std::u8string u8Path = entry.path().u8string();
                        Structure::String filePath(reinterpret_cast<const char*>(u8Path.c_str()));
                        m_model.addFile(Model::FileRecord(filePath, Model::FileType::File));
                    }
                }
            } catch (const fs::filesystem_error& e) {
                // 跳过无法访问的目录
                continue;
            }
        }
    }
}

void CompressMultipleSourcesCommand::computeRelativePaths() {
    if (m_model.getFileCount() == 0) return;

    // 策略：
    // 1. 如果源是单个目录 -> 保留目录名作为根，如 dir1/file1.txt
    // 2. 如果源是多个目录/文件 -> 尽量保留目录结构
    // 3. 如果源是单个文件 -> 只用文件名
    
    // 为每个文件找到它对应的源路径
    for (int i = 0; i < m_model.getFileCount(); ++i) {
        Model::FileRecord& record = m_model.getFile(i);
        fs::path fullPath(record.getFilePath().c_str());
        fs::path relativePath;
        
        bool found = false;
        
        // 遍历源路径，找到此文件属于哪个源
        for (int j = 0; j < m_sourcePaths.size(); ++j) {
            fs::path sourcePath(m_sourcePaths[j].c_str());
            
            if (fs::is_directory(sourcePath)) {
                // 检查文件是否在此目录下
                auto fullStr = fullPath.string();
                auto srcStr = sourcePath.string();
                
                // 确保路径分隔符统一
                std::replace(fullStr.begin(), fullStr.end(), '\\', '/');
                std::replace(srcStr.begin(), srcStr.end(), '\\', '/');
                
                if (fullStr.size() > srcStr.size() &&
                    fullStr.compare(0, srcStr.size(), srcStr) == 0 &&
                    fullStr[srcStr.size()] == '/') {
                    
                    // 文件在此源目录下
                    // 相对路径 = 源目录名 + 文件相对于源目录的路径
                    fs::path sourceDir = sourcePath.filename();  // 获取目录名（如 "dir1"）
                    fs::path relativeToSource = fs::relative(fullPath, sourcePath);
                    relativePath = sourceDir / relativeToSource;
                    
                    found = true;
                    break;
                }
            } else if (fs::is_regular_file(sourcePath)) {
                // 单个文件：只使用文件名
                if (fullPath == sourcePath) {
                    relativePath = fullPath.filename();
                    found = true;
                    break;
                }
            }
        }
        
        if (!found) {
            // 兜底：使用文件名
            relativePath = fullPath.filename();
        }
        
        Structure::String relPathStr(relativePath.string().c_str());
        record.setRelativePath(relPathStr);
    }
}

}

