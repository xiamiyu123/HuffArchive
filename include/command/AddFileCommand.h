#pragma once
#include "model/DataModel.h"
#include "structure/String.h"
#include "structure/ArrayList.h"
#include <functional>
#include <string>

namespace Command {

class AddFileCommand {
public:
    using ProgressCallback = std::function<void(float progress, const std::string& message)>;
    using CheckCancelCallback = std::function<bool()>;

    AddFileCommand(Model::DataModel* model, const Structure::String& archivePath, 
                   const Structure::ArrayList<Structure::String>& newFiles, 
                   const Structure::String& outputArchivePath);
    ~AddFileCommand();

    void execute();

    void setProgressCallback(ProgressCallback cb) { m_progressCallback = cb; }
    void setCheckCancelCallback(CheckCancelCallback cb) { m_checkCancelCallback = cb; }
    void setPassword(const std::string& password) { m_password = password; }

private:
    Model::DataModel* m_model;
    Structure::String m_archivePath;
    Structure::ArrayList<Structure::String> m_newFiles;
    Structure::String m_outputArchivePath;
    std::string m_password;
    ProgressCallback m_progressCallback;
    CheckCancelCallback m_checkCancelCallback;
};

}
