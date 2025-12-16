#pragma once
#include "model/DataModel.h"
#include "structure/String.h"
#include <functional>

namespace Command {

class DecompressCommand {
public:
    DecompressCommand(Model::DataModel* model, const Structure::String& inputPath, const Structure::String& outputDir, const std::string& password = "");
    ~DecompressCommand();

    // Execute decompression
    void execute();

    void setProgressCallback(std::function<void(float)> callback) { m_progressCallback = callback; }
    void setCheckCancelCallback(std::function<bool()> callback) { m_checkCancelCallback = callback; }

private:
    Model::DataModel* m_model;
    Structure::String m_inputPath;
    Structure::String m_outputDir;
    std::string m_password;
    std::function<void(float)> m_progressCallback;
    std::function<bool()> m_checkCancelCallback;
};

}
