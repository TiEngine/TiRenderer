#pragma once

#include <string>

namespace ti::passflow {

class Passflow;

class BasePass {
public:
    virtual void PreparePass() = 0;
    virtual void ExecutePass() = 0;

protected:
    enum class BufferType {
        ConstantBuffer,
        StructuredBuffer,
        ReadWriteBuffer
    };

    BasePass& DeclareInput(const std::string& name);
    BasePass& DeclareOutput(const std::string& name);
    BasePass& DeclareBuffer(const std::string& name);
    BasePass& DeclareTexture(const std::string& name);
    BasePass& BuildPipeline();

    BasePass& AttachInput(const std::string& name);
    BasePass& SubmitCommands();

    explicit BasePass(Passflow& passflow);
    virtual ~BasePass() = default;

    Passflow& passflow;
};

}
