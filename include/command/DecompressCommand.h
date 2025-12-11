#pragma once
#include "model/DataModel.h"
#include "structure/String.h"

namespace Command {

class DecompressCommand {
public:
    DecompressCommand(Model::DataModel* model, const Structure::String& inputPath, const Structure::String& outputDir);
    ~DecompressCommand();

    // Execute decompression
    void execute();

private:
    Model::DataModel* m_model;
    Structure::String m_inputPath;
    Structure::String m_outputDir;
};

}
