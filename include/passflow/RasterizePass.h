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

    FrameResource::PerFrame stagingPerFrameResource;
    std::vector<std::shared_ptr<DrawItem>> stagingDrawItems;
    std::unordered_map<backend::CommandRecorder*, FrameResource> frameResources;

    struct DescriptorHeaps {
        backend::DescriptorHeap* shaderResource = nullptr;
        backend::DescriptorHeap* imageSampler = nullptr;
        backend::DescriptorHeap* renderTarget = nullptr;
        backend::DescriptorHeap* depthStencil = nullptr;
    };
    std::vector<DescriptorHeaps> descriptorHeaps; //>>>?????

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
