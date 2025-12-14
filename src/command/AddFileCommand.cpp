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

    // 1. Create temporary directory
    std::error_code ec;
    std::filesystem::path tempDir = std::filesystem::temp_directory_path(ec) / "huffman_add_temp";
    if (ec) {
        // Fallback to current directory if temp fails
        tempDir = "huffman_add_temp";
    }

    if (std::filesystem::exists(tempDir)) {
        std::filesystem::remove_all(tempDir, ec);
    }
    std::filesystem::create_directories(tempDir, ec);
    
    Structure::String tempDirStr(tempDir.string().c_str());

    // 2. Decompress existing archive
    if (m_progressCallback) m_progressCallback(0.0f, "Decompressing existing archive...");
    
    DecompressCommand decompressCmd(m_model, m_archivePath, tempDirStr);
    decompressCmd.setProgressCallback([this](float p) {
        if (m_progressCallback) m_progressCallback(p * 0.4f, "Decompressing...");
    });
    decompressCmd.setCheckCancelCallback(m_checkCancelCallback);
    decompressCmd.execute();

    if (m_checkCancelCallback && m_checkCancelCallback()) {
        std::filesystem::remove_all(tempDir, ec);
        return;
    }

    // 3. Update file paths for decompressed files
    int existingCount = m_model->getFileCount();
    for (int i = 0; i < existingCount; ++i) {
        Model::FileRecord& record = m_model->getFile(i);
        // DecompressCommand sets RelativePath. We need to construct the full FilePath in temp dir.
        std::filesystem::path relPath(reinterpret_cast<const char8_t*>(record.getRelativePath().c_str()));
        std::filesystem::path fullPath = tempDir / relPath;
        record.setFilePath(Structure::String(fullPath.string().c_str()));
        record.setStatus(Model::FileStatus::Pending); // Reset status for compression
    }

    // 4. Add new files to model
    for (int i = 0; i < m_newFiles.size(); ++i) {
        Structure::String newFilePath = m_newFiles[i];
        std::filesystem::path path(reinterpret_cast<const char8_t*>(newFilePath.c_str()));
        std::string filename = path.filename().string();
        
        Model::FileRecord record(newFilePath, Model::FileType::File);
        record.setRelativePath(Structure::String(filename.c_str()));
        record.setStatus(Model::FileStatus::Pending);
        m_model->addFile(record);
    }

    // 5. Compress everything to new archive
    if (m_progressCallback) m_progressCallback(0.4f, "Compressing all files...");

    // Use a temporary file for output to prevent data loss on cancel/failure
    std::filesystem::path finalOutPath(reinterpret_cast<const char8_t*>(m_outputArchivePath.c_str()));
    std::filesystem::path tempOutPath = finalOutPath;
    tempOutPath += ".tmp";
    Structure::String tempOutPathStr(tempOutPath.string().c_str());

    CompressCommand compressCmd(m_model, tempOutPathStr);
    compressCmd.setProgressCallback([this](float p, const std::string& msg) {
        if (m_progressCallback) m_progressCallback(0.4f + p * 0.6f, msg);
    });
    compressCmd.setCheckCancelCallback(m_checkCancelCallback);
    compressCmd.execute();

    // 6. Cleanup and Finalize
    if (m_checkCancelCallback && m_checkCancelCallback()) {
        std::filesystem::remove(tempOutPath, ec);
    } else {
        // Success: Replace original file with new one
        // On Windows, rename might fail if target exists, so remove it first
        if (std::filesystem::exists(finalOutPath)) {
            std::filesystem::remove(finalOutPath, ec);
        }
        std::filesystem::rename(tempOutPath, finalOutPath, ec);
    }

    std::filesystem::remove_all(tempDir, ec);
}

}
