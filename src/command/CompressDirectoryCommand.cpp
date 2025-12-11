#include "command/CompressDirectoryCommand.h"
#include "command/CompressCommand.h"
#include <filesystem>
#include <iostream>
#include "structure/ArrayList.h"

namespace fs = std::filesystem;

namespace Command {

CompressDirectoryCommand::CompressDirectoryCommand(const Structure::String& sourcePathStr,
                                                   const Structure::String& outputPath)
    : m_sourcePath(sourcePathStr), m_outputPath(outputPath) {
}

CompressDirectoryCommand::~CompressDirectoryCommand() {
}

bool CompressDirectoryCommand::execute() {
    try {
        // 1. 验证源路径
        fs::path sourcePath(m_sourcePath.c_str());
        if (!fs::exists(sourcePath)) {
            m_errorMessage = Structure::String("Source path does not exist: ");
            m_errorMessage = m_errorMessage + m_sourcePath;
            return false;
        }

        // 2. 清空模型
        m_model.clear();

        // 3. 收集文件
        if (fs::is_regular_file(sourcePath)) {
            // 单个文件
            m_model.addFile(Model::FileRecord(m_sourcePath, Model::FileType::File));
        } else if (fs::is_directory(sourcePath)) {
            // 目录 - 递归收集所有文件
            collectFiles(sourcePath);
        } else {
            m_errorMessage = Structure::String("Source is neither a file nor a directory");
            return false;
        }

        // 检查是否有文件
        if (m_model.getFileCount() == 0) {
            m_errorMessage = Structure::String("No files to compress");
            return false;
        }

        // 4. 计算公共基路径
        fs::path basePath = computeCommonBasePath();

        // 5. 为每个文件计算相对路径
        computeRelativePaths(basePath);

        // 6. 调用底层压缩命令
        CompressCommand compressCmd(&m_model, m_outputPath);
        compressCmd.execute();

        return true;
    } catch (const std::exception& e) {
        m_errorMessage = Structure::String(e.what());
        return false;
    }
}

Structure::String CompressDirectoryCommand::getErrorMessage() const {
    return m_errorMessage;
}

void CompressDirectoryCommand::collectFiles(const fs::path& dirPath) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (fs::is_regular_file(entry)) {
                // 添加常规文件
                Structure::String filePath(entry.path().string().c_str());
                m_model.addFile(Model::FileRecord(filePath, Model::FileType::File));
            } else if (fs::is_directory(entry)) {
                // 可选：也添加目录项（用于保持目录结构）
                // 但通常压缩只需要文件即可
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during file collection: " << e.what() << std::endl;
    }
}

std::filesystem::path CompressDirectoryCommand::computeCommonBasePath() {
    if (m_model.getFileCount() == 0) {
        return fs::path(m_sourcePath.c_str()).parent_path();
    }

    if (m_model.getFileCount() == 1) {
        // 单文件情况：基路径是文件的父目录
        fs::path filePath(m_model.getFile(0).getFilePath().c_str());
        return filePath.parent_path();
    }

    // 多文件情况：找公共前缀路径
    Structure::ArrayList<fs::path> paths;
    for (int i = 0; i < m_model.getFileCount(); ++i) {
        paths.push_back(fs::path(m_model.getFile(i).getFilePath().c_str()));
    }

    // 获取第一个文件的路径分量数
    fs::path commonPath = paths[0];
    auto parts = commonPath.parent_path();

    // 比较每个路径，找到公共前缀
    for (int i = 1; i < static_cast<int>(paths.size()); ++i) {
        fs::path currentPath = paths[i].parent_path();
        
        // 逐级比较路径分量
        fs::path result;
        auto it1 = parts.begin();
        auto it2 = currentPath.begin();
        
        while (it1 != parts.end() && it2 != currentPath.end() && *it1 == *it2) {
            result /= *it1;
            ++it1;
            ++it2;
        }
        
        parts = result;
    }

    return parts;
}

void CompressDirectoryCommand::computeRelativePaths(const fs::path& basePath) {
    for (int i = 0; i < m_model.getFileCount(); ++i) {
        Model::FileRecord& record = m_model.getFile(i);
        
        fs::path filePath(record.getFilePath().c_str());
        fs::path relativePath = fs::relative(filePath, basePath);
        
        // 将路径转换为字符串，使用正斜杠（跨平台兼容）
        std::string relPathStr = relativePath.string();
        // 在 Windows 上，将反斜杠替换为正斜杠
        for (auto& c : relPathStr) {
            if (c == '\\') c = '/';
        }
        
        record.setRelativePath(Structure::String(relPathStr.c_str()));
    }
}

}
