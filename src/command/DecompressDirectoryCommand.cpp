#include "command/DecompressDirectoryCommand.h"
#include "command/DecompressCommand.h"
#include "model/DataModel.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Command {

DecompressDirectoryCommand::DecompressDirectoryCommand(const Structure::String& inputPath,
                                                       const Structure::String& outputDir)
    : m_inputPath(inputPath), m_outputDir(outputDir) {
}

DecompressDirectoryCommand::~DecompressDirectoryCommand() {
}

bool DecompressDirectoryCommand::execute() {
    try {
        // 1. 验证输入文件
        fs::path inputPath(reinterpret_cast<const char8_t*>(m_inputPath.c_str()));
        if (!fs::exists(inputPath)) {
            m_errorMessage = Structure::String("Input archive does not exist: ");
            m_errorMessage = m_errorMessage + m_inputPath;
            return false;
        }

        if (!fs::is_regular_file(inputPath)) {
            m_errorMessage = Structure::String("Input path is not a file");
            return false;
        }

        // 2. 创建输出目录
        fs::path outputPath(reinterpret_cast<const char8_t*>(m_outputDir.c_str()));
        fs::create_directories(outputPath);

        // 3. 创建数据模型（解压命令会自动填充）
        Model::DataModel model;

        // 4. 调用底层解压命令
        // DecompressCommand 会自动从归档文件中读取并恢复目录结构
        DecompressCommand decompressCmd(&model, m_inputPath, m_outputDir);
        decompressCmd.execute();

        return true;
    } catch (const std::exception& e) {
        m_errorMessage = Structure::String(e.what());
        return false;
    }
}

Structure::String DecompressDirectoryCommand::getErrorMessage() const {
    return m_errorMessage;
}

}
