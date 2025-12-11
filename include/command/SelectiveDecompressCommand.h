#pragma once

#include "structure/String.h"
#include "structure/ArrayList.h"

namespace Command {

/**
 * @brief 选择性解压命令 - 只解压指定的文件
 * 
 * 使用场景：
 * - 从大型压缩包中只提取需要的几个文件
 * - 避免解压整个归档文件，节省时间和空间
 * - 按需加载特定文件
 * 
 * 使用方法：
 * @code
 * // 创建文件列表
 * Structure::ArrayList<Structure::String> filesToExtract;
 * filesToExtract.add(Structure::String("src/main.cpp"));
 * filesToExtract.add(Structure::String("data/config.txt"));
 * 
 * // 执行选择性解压
 * SelectiveDecompressCommand cmd(
 *     Structure::String("archive.huff"),
 *     Structure::String("output_dir"),
 *     filesToExtract
 * );
 * if (!cmd.execute()) {
 *     std::cerr << "Error: " << cmd.getErrorMessage().c_str() << std::endl;
 * }
 * @endcode
 */
class SelectiveDecompressCommand {
public:
    /**
     * @param inputPath 压缩包路径
     * @param outputDir 解压输出目录
     * @param fileFilter 需要解压的文件列表（相对路径）
     *                   如果为空，则解压所有文件（等同于 DecompressDirectoryCommand）
     */
    SelectiveDecompressCommand(const Structure::String& inputPath,
                              const Structure::String& outputDir,
                              const Structure::ArrayList<Structure::String>& fileFilter);
    ~SelectiveDecompressCommand();

    /**
     * @brief 执行选择性解压
     * @return true 成功，false 失败
     */
    bool execute();

    /**
     * @brief 获取错误信息
     */
    Structure::String getErrorMessage() const;

    /**
     * @brief 获取实际解压的文件数量
     */
    int getExtractedCount() const;

    /**
     * @brief 获取跳过的文件数量（在归档中找不到）
     */
    int getSkippedCount() const;

private:
    Structure::String m_inputPath;
    Structure::String m_outputDir;
    Structure::ArrayList<Structure::String> m_fileFilter;
    Structure::String m_errorMessage;
    int m_extractedCount;
    int m_skippedCount;

    /**
     * @brief 检查文件路径是否在过滤列表中
     * @param filePath 要检查的文件路径（相对路径）
     * @return true 如果在过滤列表中，false 否则
     */
    bool isFileInFilter(const Structure::String& filePath) const;
};

}
