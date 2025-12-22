#include "command/AddFileCommand.h"
#include "command/DecompressCommand.h"
#include "command/CompressCommand.h"
#include <filesystem>
#include <iostream>

namespace Command {

AddFileCommand::AddFileCommand(Model::DataModel* model, const Structure::String& archivePath, 
                               const Structure::ArrayList<Structure::String>& newFiles, 
                               const Structure::String& outputArchivePath)
    : m_model(model), m_archivePath(archivePath), m_newFiles(newFiles), m_outputArchivePath(outputArchivePath) {
}

AddFileCommand::~AddFileCommand() {
}

void AddFileCommand::execute() {
    if (!m_model) return;

    // 1. 创建临时目录
    std::error_code ec;
    std::filesystem::path tempDir = std::filesystem::temp_directory_path(ec) / "huffman_add_temp";
    if (ec) {
        // 如果临时目录失败，回退到当前目录
        tempDir = "huffman_add_temp";
    }

    if (std::filesystem::exists(tempDir)) {
        std::filesystem::remove_all(tempDir, ec);
    }
    std::filesystem::create_directories(tempDir, ec);
    
    std::u8string u8TempDir = tempDir.u8string();
    Structure::String tempDirStr(reinterpret_cast<const char*>(u8TempDir.c_str()));

    // 2. 解压现有归档
    if (m_progressCallback) m_progressCallback(0.0f, "正在解压现有归档...");
    
    DecompressCommand decompressCmd(m_model, m_archivePath, tempDirStr, m_password);
    decompressCmd.setProgressCallback([this](float p) {
        if (m_progressCallback) m_progressCallback(p * 0.4f, "正在解压...");
    });
    decompressCmd.setCheckCancelCallback(m_checkCancelCallback);
    decompressCmd.execute();

    if (m_checkCancelCallback && m_checkCancelCallback()) {
        std::filesystem::remove_all(tempDir, ec);
        return;
    }

    // 3. 更新解压文件的文件路径
    int existingCount = m_model->getFileCount();
    for (int i = 0; i < existingCount; ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        // DecompressCommand 设置相对路径。我们需要在临时目录中构建完整的文件路径。
        std::filesystem::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
        std::filesystem::path fullPath = tempDir / relPath;
        std::u8string u8FullPath = fullPath.u8string();
        record.setFilePath(Structure::String(reinterpret_cast<const char*>(u8FullPath.c_str())));
        record.setStatus(Model::FileStatus::Pending); // 为压缩重置状态
    }

    // 4. 将新文件添加到模型
    for (int i = 0; i < m_newFiles.size(); ++i) {
        Structure::String newFilePath = m_newFiles[i];
        std::filesystem::path path(reinterpret_cast<const char8_t*>(newFilePath.c_str()));
        std::u8string u8Filename = path.filename().u8string();
        std::string filename(reinterpret_cast<const char*>(u8Filename.c_str()));
        Structure::String relPath(filename.c_str());
        
        // 检查是否存在同名文件，如果存在则移除旧记录（覆盖）
        for (int j = 0; j < m_model->getFileCount(); ++j) {
            if (m_model->getFile(j).getRelativePath() == relPath) {
                m_model->removeFile(j);
                j--; // 调整索引
            }
        }
        
        Model::FileRecord record(newFilePath, Model::FileType::File);
        record.setRelativePath(relPath);
        record.setStatus(Model::FileStatus::Pending);
        m_model->addFile(record);
    }

    // 5. 将所有内容压缩到新归档
    if (m_progressCallback) m_progressCallback(0.4f, "正在压缩所有文件...");

    // 使用临时文件进行输出，以防止在取消/失败时丢失数据
    std::filesystem::path finalOutPath(reinterpret_cast<const char8_t*>(m_outputArchivePath.c_str()));
    std::filesystem::path tempOutPath = finalOutPath;
    tempOutPath += ".tmp";
    std::u8string u8TempOutPath = tempOutPath.u8string();
    Structure::String tempOutPathStr(reinterpret_cast<const char*>(u8TempOutPath.c_str()));

    CompressCommand compressCmd(m_model, tempOutPathStr);
    compressCmd.setPassword(Structure::String(m_password.c_str()));
    compressCmd.setProgressCallback([this](float p, const std::string& msg) {
        if (m_progressCallback) m_progressCallback(0.4f + p * 0.6f, msg);
    });
    compressCmd.setCheckCancelCallback(m_checkCancelCallback);
    compressCmd.execute();

    // 6. 清理并完成
    if (m_checkCancelCallback && m_checkCancelCallback()) {
        std::filesystem::remove(tempOutPath, ec);
    } else {
        // 成功：用新文件替换原始文件
        // 在 Windows 上，如果目标存在，重命名可能会失败，所以先删除它
        if (std::filesystem::exists(finalOutPath)) {
            std::filesystem::remove(finalOutPath, ec);
        }
        std::filesystem::rename(tempOutPath, finalOutPath, ec);
    }

    std::filesystem::remove_all(tempDir, ec);
}

}
