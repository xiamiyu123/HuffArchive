#pragma once
#include "model/DataModel.h"
#include "structure/String.h"

namespace Command {

class CompressCommand {
public:
    CompressCommand(Model::DataModel* model, const Structure::String& outputPath);
    ~CompressCommand();

    // Execute compression
    void execute();

private:
    Model::DataModel* m_model;
    Structure::String m_outputPath;
};

}
