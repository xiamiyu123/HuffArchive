#pragma once

#include "model/DataModel.h"
#include "structure/String.h"
#include "structure/ArrayList.h"

namespace Command {

/**
 * @brief 最高层压缩命令 - 支持多个源路径（文件和目录混合）
 * 
 * 这是最高抽象层级的命令类，专为 GUI 设计。用户可以：
 * 1. 添加多个文件（来自不同位置）
 * 2. 添加多个目录（来自不同位置）
 * 3. 混合添加文件和目录
 * 
 * 命令会自动：
 * - 递归展开所有目录
 * - 智能计算公共基路径
 * - 为每个文件设置合理的相对路径
 * - 调用底层 CompressCommand 执行压缩
 * 
 * 示例：
 *   sources: ["C:/docs/report.pdf", "C:/images/", "D:/music/song.mp3"]
 *   → 自动处理跨盘符、不同目录的情况
 *   → 在压缩包内使用合理的相对路径结构
 */
class CompressMultipleSourcesCommand {
public:
    /**
     * @param sourcePaths 源文件/目录路径列表（可以混合）
     * @param outputPath 输出压缩包路径
     */
    CompressMultipleSourcesCommand(const Structure::ArrayList<Structure::String>& sourcePaths,
                                   const Structure::String& outputPath);
    ~CompressMultipleSourcesCommand();

    /**
     * @brief 执行压缩
     * @return true 成功，false 失败
     */
    bool execute();

    /**
     * @brief 获取错误信息
     */
    Structure::String getErrorMessage() const;

    /**
     * @brief 获取数据模型（用于查看进度等）
     */
    const Model::DataModel& getModel() const;

private:
    Structure::ArrayList<Structure::String> m_sourcePaths;
    Structure::String m_outputPath;
    Structure::String m_errorMessage;
    Model::DataModel m_model;

    /**
     * @brief 收集所有源路径中的文件
     */
    void collectAllFiles();

    /**
     * @brief 计算所有文件的公共基路径
     * 
     * 策略：
     * - 如果所有文件在同一目录树下：使用最近公共祖先
     * - 如果跨越多个盘符/根目录：使用文件/目录名作为相对路径
     */
    void computeRelativePaths();
};

}
