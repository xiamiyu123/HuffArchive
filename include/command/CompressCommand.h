#pragma once
#include "model/DataModel.h"
#include "structure/String.h"
#include <functional>
#include <string>

namespace Command {

class CompressCommand {
public:
    using ProgressCallback = std::function<void(float progress, const std::string& message)>;
    using CheckCancelCallback = std::function<bool()>;

    CompressCommand(Model::DataModel* model, const Structure::String& outputPath);
    ~CompressCommand();

    // Execute compression
    void execute();

    void setProgressCallback(ProgressCallback cb) { m_progressCallback = cb; }
    void setCheckCancelCallback(CheckCancelCallback cb) { m_checkCancelCallback = cb; }
    void setPassword(const Structure::String& password) { m_password = password; }

private:
    Model::DataModel* m_model;
    Structure::String m_outputPath;
    Structure::String m_password;
    ProgressCallback m_progressCallback;
    CheckCancelCallback m_checkCancelCallback;
};

}
