#pragma once

#include "model/DataModel.h"
#include "structure/String.h"
#include "structure/ArrayList.h"
#include <filesystem>

namespace Command {

/**
 * @brief 高层压缩命令 - 自动处理文件收集和相对路径推断
 * 
 * 这是一个更高抽象层级的命令类，用户只需提供：
 * 1. 源文件/目录路径
 * 2. 输出压缩包路径
 * 
 * 命令会自动：
 * - 递归收集文件（如果是目录）
 * - 计算最小公共基路径（common base path）
 * - 为每个文件设置正确的相对路径
 * - 调用底层 CompressCommand 执行压缩
 */
class CompressDirectoryCommand {
public:
    /**
     * @param sourcePathStr 源文件或目录路径（可以是绝对路径或相对路径）
     * @param outputPath 输出压缩包路径
     */
    CompressDirectoryCommand(const Structure::String& sourcePathStr, 
                             const Structure::String& outputPath);
    ~CompressDirectoryCommand();

    /**
     * @brief 执行压缩
     * @return true 成功，false 失败
     */
    bool execute();

    /**
     * @brief 获取错误信息
     */
    Structure::String getErrorMessage() const;

private:
    Structure::String m_sourcePath;
    Structure::String m_outputPath;
    Structure::String m_errorMessage;
    Model::DataModel m_model;

    /**
     * @brief 递归收集目录中的所有文件
     * @param dirPath 目录路径
     */
    void collectFiles(const std::filesystem::path& dirPath);

    /**
     * @brief 计算所有文件的公共基路径
     * @return 公共基路径
     */
    std::filesystem::path computeCommonBasePath();

    /**
     * @brief 为文件列表中的每个文件计算相对路径
     * @param basePath 基路径
     */
    void computeRelativePaths(const std::filesystem::path& basePath);
};

}
