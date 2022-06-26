#pragma once

#include "backend/BackendContext.h"

namespace ti::passflow {

class Passflow;

class BasePass {
public:
    virtual void PreparePass() = 0;
    virtual void BeginPass(backend::CommandRecorder& recorder) = 0;
    virtual void ExecutePass(backend::CommandRecorder& recorder) = 0;
    virtual void EndPass(backend::CommandRecorder& recorder) = 0;

    virtual ~BasePass() = default;

protected:
    enum class BufferType {
        ConstantBuffer,
        StructuredBuffer,
        ReadWriteBuffer
    };

    BasePass& BuildPipeline();
    BasePass& SubmitCommands();

    explicit BasePass(Passflow& passflow);

    Passflow& passflow;
};

}
