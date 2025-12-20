#pragma once

#include "model/DataModel.h"
#include "structure/String.h"
#include "structure/ArrayList.h"
#include <functional>
#include <string>

namespace Command {

class DeleteFileCommand {
public:
    DeleteFileCommand(Model::DataModel* model, const Structure::String& archivePath, 
                      const Structure::ArrayList<Structure::String>& filesToDelete, 
                      const Structure::String& outputArchivePath);
    ~DeleteFileCommand();

    void execute();
    
    void setProgressCallback(std::function<void(float, const std::string&)> callback) {
        m_progressCallback = callback;
    }
    
    void setCheckCancelCallback(std::function<bool()> callback) {
        m_checkCancelCallback = callback;
    }

private:
    Model::DataModel* m_model;
    Structure::String m_archivePath;
    Structure::ArrayList<Structure::String> m_filesToDelete;
    Structure::String m_outputArchivePath;
    
    std::function<void(float, const std::string&)> m_progressCallback;
    std::function<bool()> m_checkCancelCallback;
};

}
