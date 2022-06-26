#pragma once

#include "backend/BackendContext.h"

namespace ti::passflow {

class Passflow;

class BasePass {
public:
    virtual ~BasePass() = default;

    virtual void PreparePass() = 0;
    virtual void BeginPass(backend::CommandRecorder& recorder) = 0;
    virtual void ExecutePass(backend::CommandRecorder& recorder) = 0;
    virtual void EndPass(backend::CommandRecorder& recorder) = 0;

protected:
    explicit BasePass(Passflow& passflow);

    Passflow& passflow;
};

}
