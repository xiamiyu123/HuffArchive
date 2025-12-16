#pragma once

#include "structure/String.h"
#include <functional>

namespace Command {

/**
 * @brief 高层解压命令 - 自动处理文件解压和目录结构恢复
 * 
 * 使用方法非常简单：
 * 1. 提供压缩包路径
 * 2. 提供输出目录
 * 3. 调用 execute()
 * 
 * 命令会自动：
 * - 验证压缩包格式
 * - 恢复原始的目录结构
 * - 将所有文件解压到输出目录
 */
class DecompressDirectoryCommand {
public:
    /**
     * @param inputPath 压缩包路径
     * @param outputDir 解压输出目录
     */
    DecompressDirectoryCommand(const Structure::String& inputPath,
                              const Structure::String& outputDir,
                              const std::string& password = "");
    ~DecompressDirectoryCommand();

    /**
     * @brief 执行解压
     * @return true 成功，false 失败
     */
    bool execute();

    /**
     * @brief 获取错误信息
     */
    Structure::String getErrorMessage() const;

    void setProgressCallback(std::function<void(float)> callback) { m_progressCallback = callback; }
    void setCheckCancelCallback(std::function<bool()> callback) { m_checkCancelCallback = callback; }

private:
    Structure::String m_inputPath;
    Structure::String m_outputDir;
    std::string m_password;
    Structure::String m_errorMessage;
    std::function<void(float)> m_progressCallback;
    std::function<bool()> m_checkCancelCallback;
};

}
