#pragma once

#include "BasePass.h"
#include "FrameResources.h"

namespace ti::passflow {

class RasterizePass : public BasePass {
public:
    ~RasterizePass() override;

    virtual unsigned int AddDrawItem(std::weak_ptr<DrawItem> item);
    virtual bool VerifyDrawItemIndex(unsigned int index);

    virtual void ImportResource(const std::string& name,
        std::shared_ptr<BaseConstantBuffer> constantBuffer);
    virtual void ImportResource(const std::string& name,
        std::shared_ptr<BaseStructuredBuffer> structuredBuffer);
    virtual void ImportResource(const std::string& name,
        std::shared_ptr<BaseReadWriteBuffer> readWriteBuffer);

    virtual void ImportResource(const std::string& name,
        std::shared_ptr<Texture2D> texture2D);
    virtual void ImportResource(const std::string& name,
        std::shared_ptr<ReadWriteTexture2D> readWriteTexture2D);

    virtual void ImportOutput(const std::string& name,
        std::shared_ptr<ColorOutput> colorOutput);
    virtual void ImportOutput(const std::string& name,
        std::shared_ptr<DepthStencilOutput> depthStencilOutput);
    virtual void ImportOutput(const std::string& name,
        std::shared_ptr<DisplayPresentOutput> displayPresentOutput);

    void ClearFrameResources();

protected:
    explicit RasterizePass(Passflow& passflow);

    void InitializePipeline(backend::Device* device);
    void DeclareInput(const InputProperties& properties);
    void DeclareOutput(const OutputProperties& properties);
    void DeclareProgram(const ProgramProperties& properties);
    void DeclareResource(const ShaderResourceProperties& properties);
    void BuildPipeline();
    void CleanPipeline();

    void ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex);
    void ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex);

    void UpdateDrawItems(unsigned int bufferingIndex);
    void UpdateFrameResources(unsigned int bufferingIndex);

    FrameResources& AcquireFrameResource(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResource();

private:
    backend::Device* device = nullptr; // Not owned!

    backend::PipelineState* pipelineState = nullptr;
    backend::PipelineLayout* pipelineLayout = nullptr;

    backend::InputVertexAttributes* inputVertexAttributes = nullptr;
    backend::InputIndexAttribute* inputIndexAttribute = nullptr;

    std::map<ShaderStage, backend::Shader*> programShaders;
    std::map<uint8_t, backend::DescriptorGroup*> descriptorGroups;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;
    std::vector<DynamicDescriptorManager> renderTargetDescriptorHeaps;
    std::vector<DynamicDescriptorManager> depthStencilDescriptorHeaps;

    struct RasterizePipelineCounters final {
        bool allowMultiObjects = false;
        unsigned int colorOutputCount = 0;
        unsigned int depthStencilOutputCount = 0;
        unsigned int reservedObjectsCount = 1;
        unsigned int objectShaderResourcesCount = 0;
        unsigned int shaderResourcesCount = 0;
        unsigned int imageSamplersCount = 0;
    } rasterizePipelineCounters;

    std::vector<FrameResources> frameResources;
};

}
