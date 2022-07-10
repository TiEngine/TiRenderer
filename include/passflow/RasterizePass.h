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

    void InitializePipeline(backend::Device* device);
    void DeclareInput(const InputProperties& properties);
    void DeclareOutput(const OutputProperties& properties);
    void DeclareProgram(const ProgramProperties& properties);
    void DeclareResource(const ShaderResourceProperties& properties);
    void BuildPipeline();
    void CleanPipeline();

    struct RasterizePipelineCounters final {
        bool allowMultiObjects = false;
        unsigned int colorOutputCount = 0;
        unsigned int depthStencilOutputCount = 0;
        unsigned int reservedObjectsCount = 1;
    } rasterizePipelineCounters;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;
    std::vector<DynamicDescriptorManager> renderTargetDescriptorHeaps;
    std::vector<DynamicDescriptorManager> depthStencilDescriptorHeaps;


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
