#include "command/DeleteFileCommand.h"
#include "command/DecompressCommand.h"
#include "command/CompressCommand.h"
#include <filesystem>
#include <iostream>

namespace Command {

DeleteFileCommand::DeleteFileCommand(Model::DataModel* model, const Structure::String& archivePath, 
                                     const Structure::ArrayList<Structure::String>& filesToDelete, 
                                     const Structure::String& outputArchivePath)
    : m_model(model), m_archivePath(archivePath), m_filesToDelete(filesToDelete), m_outputArchivePath(outputArchivePath) {
}

DeleteFileCommand::~DeleteFileCommand() {
}

void DeleteFileCommand::execute() {
    if (!m_model) return;

    // 1. 创建临时目录
    std::error_code ec;
    std::filesystem::path tempDir = std::filesystem::temp_directory_path(ec) / "huffman_del_temp";
    if (ec) {
        tempDir = "huffman_del_temp";
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

    // 3. 更新解压文件的文件路径，并移除要删除的文件
    int existingCount = m_model->getFileCount();
    // 我们需要倒序遍历或者使用一个新的列表来存储保留的文件，因为 removeFile 会改变索引
    // 这里我们选择直接在 model 上操作，倒序遍历
    
    for (int i = existingCount - 1; i >= 0; --i) {
        Model::FileRecord& record = m_model->getFile(i);
        
        // 检查是否在删除列表中
        bool shouldDelete = false;
        for (int j = 0; j < m_filesToDelete.size(); ++j) {
            if (record.getRelativePath() == m_filesToDelete[j]) {
                shouldDelete = true;
                break;
            }
        }
        
        if (shouldDelete) {
            m_model->removeFile(i);
        } else {
            // 更新路径指向临时文件
            std::filesystem::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
            std::filesystem::path fullPath = tempDir / relPath;
            std::u8string u8FullPath = fullPath.u8string();
            record.setFilePath(Structure::String(reinterpret_cast<const char*>(u8FullPath.c_str())));
            record.setStatus(Model::FileStatus::Pending);
        }
    }

    // 4. 将剩余内容压缩到新归档
    if (m_progressCallback) m_progressCallback(0.4f, "正在重新压缩...");

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

    // 5. 清理并完成
    if (m_checkCancelCallback && m_checkCancelCallback()) {
        std::filesystem::remove(tempOutPath, ec);
    } else {
        if (std::filesystem::exists(finalOutPath)) {
            std::filesystem::remove(finalOutPath, ec);
        }
        std::filesystem::rename(tempOutPath, finalOutPath, ec);
    }
    
    std::filesystem::remove_all(tempDir, ec);
}

}
