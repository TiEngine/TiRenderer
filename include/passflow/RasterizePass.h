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
        unsigned int objectShaderResourcesCount = 0; // Update each object draw call.
        unsigned int shaderResourcesCount = 0;       // Update each execute pass.
        unsigned int imageSamplersCount = 0;         // Update each execute pass.
    } rasterizePipelineCounters;

    std::vector<std::unique_ptr<DynamicDescriptorManager>> shaderResourceDescriptorHeaps;
    std::vector<std::unique_ptr<DynamicDescriptorManager>> imageSamplerDescriptorHeaps;
    std::vector<std::unique_ptr<DynamicDescriptorManager>> renderTargetDescriptorHeaps;
    std::vector<std::unique_ptr<DynamicDescriptorManager>> depthStencilDescriptorHeaps;
    std::vector<std::unique_ptr<DynamicDescriptorManager>> delayCleanupDescriptorHeaps;

    // Shader resource descriptor heap expansion operation (reservedObjectsCount changed will
    // trigger it). This function should be called in the OnBeforePass function.
    void CheckAndExpandShaderResourceDescriptorHeaps(unsigned int currentBufferingIndex);
    // Because the GPU commands may not have been executed when the expansion operation occurs,
    // so delay the cleanup of previous descriptor heap until it is determined that the commands
    // in the GPU have been executed. This function should be called in the OnAfterPass function.
    void ClearDelayCleanupDescriptorHeaps();

    //FrameResource::PerFrame stagingPerFrameResource;
    std::vector<std::shared_ptr<DrawItem>> stagingDrawItems;
    //std::unordered_map<backend::CommandRecorder*, FrameResource> frameResources;

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
