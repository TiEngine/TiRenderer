#pragma once

#include "BasePass.h"
#include "FrameResource.h"

namespace ti::passflow {

class RasterizePass : public BasePass {
public:
    ~RasterizePass() override;

    virtual void AddDrawItem(std::shared_ptr<DrawItem> item);

protected:
    explicit RasterizePass(Passflow& passflow);

    void CleanupPipeline();

    RasterizePass& InitializePipeline(backend::Device* device);
    RasterizePass& DeclareInput(const InputProperties& properties);
    RasterizePass& DeclareOutput(const OutputProperties& properties);
    RasterizePass& DeclareProgram(const ProgramProperties& properties);
    RasterizePass& DeclareResource(const ShaderResourceProperties& properties);
    void BuildPipeline();

    FrameResource::PerFrame stagingPerFrameResource;
    std::vector<std::shared_ptr<DrawItem>> stagingDrawItems;
    std::unordered_map<backend::CommandRecorder*, FrameResource> frameResources;

private:
    backend::Device* device = nullptr; // Not owned!
    backend::PipelineState* pipelineState = nullptr;
    backend::PipelineLayout* pipelineLayout = nullptr;
    backend::InputVertexAttributes* inputVertexAttributes = nullptr;
    backend::InputIndexAttribute* inputIndexAttribute = nullptr;
    std::map<ShaderStage, backend::Shader*> programShaders;
    std::map<uint8_t, backend::DescriptorGroup*> descriptorGroups;
};

}
