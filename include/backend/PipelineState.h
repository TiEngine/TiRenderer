#pragma once

#include "InputIndexAttribute.h"
#include "InputVertexAttributes.h"

namespace ti::backend {
class PipelineState {
public:
    struct Description {
        unsigned int reserved = 0;
    };

    // Resource state
    virtual void SetPipelineLayout(PipelineLayout* layout) = 0;

    // Input state
    virtual void SetIndexAssembly(InputIndexAttribute* iia) = 0;
    virtual void SetVertexAssembly(InputVertexAttributes* iva) = 0;

    // Output state
    virtual void SetColorAttachment(unsigned int location, BasicFormat format) = 0;
    virtual void SetDepthStencilAttachment(BasicFormat format) = 0;

    // Shader program
    virtual void SetShader(std::unordered_map<ShaderStage, Shader*> shaders) = 0;

    // Non-programming state
    virtual void SetRasterizerState(RasterizerState state) = 0;
    virtual void SetRasterizerStateFillMode(FillMode mode) = 0;
    virtual void SetRasterizerStateCullMode(CullMode mode) = 0;
    virtual void SetMSAA(MSAA msaa) = 0;

    // Build PSO
    virtual void BuildState() = 0;

protected:
    PipelineState() = default;
    virtual ~PipelineState() = default;
};
}
