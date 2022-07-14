#include "passflow/RasterizePass.h"
#include "passflow/Passflow.h"

namespace {

LOG_TAG(RasterizePass)

}

namespace ti::passflow {

RasterizePass::RasterizePass(Passflow& passflow) : BasePass(passflow)
{
    ClearFrameResources();
}

RasterizePass::~RasterizePass()
{
    CleanPipeline();
}

unsigned int RasterizePass::AddDrawItem(std::weak_ptr<DrawItem> item)
{
    if (auto lockedItem = item.lock()) {
        AcquireStagingFrameResource().drawItems.emplace_back(lockedItem);
        return static_cast<unsigned int>(AcquireStagingFrameResource().drawItems.size() - 1);
    }
    TI_LOG_W(TAG, "Draw item has been released and cannot be added to pass for rendering.");
    return std::numeric_limits<unsigned int>::max();
}

bool RasterizePass::VerifyDrawItemIndex(unsigned int index)
{
    return (index < AcquireStagingFrameResource().drawItems.size()) ? true : false;
}

void RasterizePass::ImportResource(const std::string& name,
    std::shared_ptr<BaseConstantBuffer> constantBuffer)
{
    AcquireStagingFrameResource().oneFrameResources.constantBuffers[name] = constantBuffer;
}

void RasterizePass::ImportResource(const std::string& name,
    std::shared_ptr<BaseStructuredBuffer> structuredBuffer)
{
    AcquireStagingFrameResource().oneFrameResources.structuredBuffers[name] = structuredBuffer;
}

void RasterizePass::ImportResource(const std::string& name,
    std::shared_ptr<BaseReadWriteBuffer> readWriteBuffer)
{
    AcquireStagingFrameResource().oneFrameResources.readWriteBuffers[name] = readWriteBuffer;
}

void RasterizePass::ImportResource(const std::string& name,
    std::shared_ptr<Texture2D> texture2D)
{
    AcquireStagingFrameResource().oneFrameResources.texture2Ds[name] = texture2D;
}

void RasterizePass::ImportResource(const std::string& name,
    std::shared_ptr<ReadWriteTexture2D> readWriteTexture2D)
{
    AcquireStagingFrameResource().oneFrameResources.readWriteTexture2Ds[name] = readWriteTexture2D;
}

void RasterizePass::ImportOutput(const std::string& name,
    std::shared_ptr<ColorOutput> colorOutput)
{
    AcquireStagingFrameResource().oneFrameResources.colorOutputs[name] = colorOutput;
}

void RasterizePass::ImportOutput(const std::string& name,
    std::shared_ptr<DepthStencilOutput> depthStencilOutput)
{
    AcquireStagingFrameResource().oneFrameResources.depthStencilOutputs[name] = depthStencilOutput;
}

void RasterizePass::ImportOutput(const std::string& name,
    std::shared_ptr<DisplayPresentOutput> displayPresentOutput)
{
    AcquireStagingFrameResource().oneFrameResources.swapchainOutputs[name] = displayPresentOutput;
}

void RasterizePass::InitializePipeline(backend::Device* device)
{
    this->device = device;

    pipelineState = device->CreatePipelineState();
    pipelineLayout = device->CreatePipelineLayout();
}

void RasterizePass::DeclareInput(const InputProperties& properties)
{
    inputVertexAttributes = device->CreateInputVertexAttributes();
    for (const auto& attribute : properties.vertexAttributes) {
        inputVertexAttributes->AddAttribute(attribute);
    }
    inputIndexAttribute = device->CreateInputIndexAttribute();
    inputIndexAttribute->SetAttribute(properties.indexAttribute);

    rasterizePipelineCounters.allowMultiObjects = properties.multipleObjects;
    if (rasterizePipelineCounters.allowMultiObjects) {
        // The number of objects reserved initially.
        // If the number of objects added by calling AddDrawItem exceeds it, it will
        // be doubled when the reserved descriptors in descriptor heap is not enough.
        rasterizePipelineCounters.reservedObjectsCount = 16;
    }
}

void RasterizePass::DeclareOutput(const OutputProperties& properties)
{
    rasterizePipelineCounters.colorOutputCount = 0;
    rasterizePipelineCounters.depthStencilOutputCount = 0;

    for (const auto& [outputSlot, outputAttribute] : properties.targets) {
        if (outputSlot == OutputProperties::OutputSlot::DS) {
            pipelineState->SetDepthStencilOutputFormat(outputAttribute.imagePixelFormat);
            rasterizePipelineCounters.depthStencilOutputCount = 1;
        } else { // Color: C0,...,C7
            pipelineState->SetColorOutputFormat(
                common::EnumCast(outputSlot), outputAttribute.imagePixelFormat);
            rasterizePipelineCounters.colorOutputCount++;
        }
    }
}

void RasterizePass::DeclareProgram(const ProgramProperties& properties)
{
    for (const auto& [shaderStage, shaderSource] : properties.shaders) {
        programShaders[shaderStage] = device->CreateShader({ shaderStage, shaderSource });
    }
}

void RasterizePass::DeclareResource(const ShaderResourceProperties& properties)
{
    rasterizePipelineCounters.objectShaderResourcesCount = 0;
    rasterizePipelineCounters.shaderResourcesCount = 0;
    rasterizePipelineCounters.imageSamplersCount = 0;

    pipelineLayout = device->CreatePipelineLayout();
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = common::EnumCast(resourceSpace);
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& attribute : resourceAttributes) {
            if (resourceSpace == ShaderResourceProperties::ResourceSpace::PerObject) {
                if (attribute.resourceType == DescriptorType::ShaderResource) {
                    rasterizePipelineCounters.objectShaderResourcesCount++;
                } else {
                    TI_LOG_F(TAG, "PerObject only can have the resource type of ShaderResource.");
                    // Note that continue will cause the descriptor group to be out of order,
                    // but there is no way to solve it, so use LOG_F to print fatal error.
                    continue;
                }
            } else {
                if (attribute.resourceType == DescriptorType::ShaderResource) {
                    rasterizePipelineCounters.shaderResourcesCount++;
                } else if (attribute.resourceType == DescriptorType::ImageSampler) {
                    rasterizePipelineCounters.imageSamplersCount++;
                } else {
                    TI_LOG_F(TAG, "Resource only can be the type of ShaderResource/ImageSampler.");
                    continue; // NB: Ditto.
                }
            }

            if (attribute.bindingPointCount == 1) {
                group->AddDescriptor(
                    attribute.resourceType,
                    attribute.baseBindingPoint,
                    attribute.resourceVisibility);
            } else if (attribute.bindingPointCount > 1) {
                unsigned int begBp = attribute.baseBindingPoint;
                unsigned int endBp = attribute.baseBindingPoint +
                                     attribute.bindingPointCount - 1;
                group->AddDescriptors(
                    attribute.resourceType,
                    std::make_pair(begBp, endBp),
                    attribute.resourceVisibility);
            }
        }
        pipelineLayout->AddGroup(group);
    }
    pipelineLayout->BuildLayout();
}

void RasterizePass::BuildPipeline()
{
    pipelineState->SetPipelineLayout(pipelineLayout);
    pipelineState->SetIndexAssembly(inputIndexAttribute);
    pipelineState->SetVertexAssembly(inputVertexAttributes);
    for (const auto& [stage, shader] : programShaders) {
        pipelineState->SetShader(stage, shader);
    }
    pipelineState->BuildState();

    unsigned int mbc = passflow.GetMultipleBufferingCount();
    shaderResourceDescriptorHeaps.resize(mbc, { device, DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(mbc, { device, DescriptorType::ImageSampler });
    renderTargetDescriptorHeaps.resize(mbc, { device, DescriptorType::ColorOutput });
    depthStencilDescriptorHeaps.resize(mbc, { device, DescriptorType::DepthStencil });
    for (unsigned int index = 0; index < mbc; index++) {
        ReserveEnoughAllTypesDescriptors(index);
    }
}

void RasterizePass::CleanPipeline()
{
    if (device) {
        if (inputVertexAttributes) {
            device->DestroyInputVertexAttributes(inputVertexAttributes);
        }
        inputVertexAttributes = nullptr;
        if (inputIndexAttribute) {
            device->DestroyInputIndexAttribute(inputIndexAttribute);
        }
        inputIndexAttribute = nullptr;

        for (const auto& shader : programShaders) {
            device->DestroyShader(shader.second);
        }
        programShaders.clear();

        for (const auto& group : descriptorGroups) {
            device->DestroyDescriptorGroup(group.second);
        }
        descriptorGroups.clear();

        if (pipelineLayout) {
            device->DestroyPipelineLayout(pipelineLayout);
        }
        pipelineLayout = nullptr;
        if (pipelineState) {
            device->DestroyPipelineState(pipelineState);
        }
        pipelineState = nullptr;

        shaderResourceDescriptorHeaps.clear();
        imageSamplerDescriptorHeaps.clear();
        renderTargetDescriptorHeaps.clear();
        depthStencilDescriptorHeaps.clear();

        // Not owned the device, so set it to null simply.
        device = nullptr;

        // Reset recorded counters variable to default values.
        rasterizePipelineCounters = {};
    }
}

void RasterizePass::ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex)
{
    do {
        if (rasterizePipelineCounters.reservedObjectsCount >=
            AcquireStagingFrameResource().drawItems.size()) {
            break;
        }
    } while (rasterizePipelineCounters.reservedObjectsCount <<= 1);

    if (rasterizePipelineCounters.reservedObjectsCount == 0) {
        TI_LOG_F(TAG, "Draw items count overflow and no enough descriptors!");
    }

    shaderResourceDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.shaderResourcesCount +
        rasterizePipelineCounters.reservedObjectsCount *
        rasterizePipelineCounters.objectShaderResourcesCount);
}

void RasterizePass::ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex)
{
    ReserveEnoughShaderResourceDescriptors(bufferingIndex);

    imageSamplerDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.imageSamplersCount);

    renderTargetDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.colorOutputCount);

    depthStencilDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.depthStencilOutputCount);
}

void RasterizePass::UpdateDrawItems(unsigned int bufferingIndex)
{
    Resources& updating = AcquireFrameResource(bufferingIndex);
    Resources& staging = AcquireStagingFrameResource();

    auto Updater = [](auto& updating, auto& staging) {
        updating.clear();
        updating.swap(staging);
    };

    Updater(updating.drawItems,
             staging.drawItems);
    Updater(updating.drawItemsExtraResources,
             staging.drawItemsExtraResources);
}

void RasterizePass::UpdateFrameResources(unsigned int bufferingIndex)
{
    Resources& updating = AcquireFrameResource(bufferingIndex);
    Resources& staging = AcquireStagingFrameResource();

    auto Updater = [](auto& updating, auto& staging) {
        for (const auto& [name, resource] : updating) {
            staging[name] = resource;
        }
        updating.clear();
    };

    Updater(updating.oneFrameResources.constantBuffers,
             staging.oneFrameResources.constantBuffers);
    Updater(updating.oneFrameResources.structuredBuffers,
             staging.oneFrameResources.structuredBuffers);
    Updater(updating.oneFrameResources.readWriteBuffers,
             staging.oneFrameResources.readWriteBuffers);

    Updater(updating.oneFrameResources.texture2Ds,
             staging.oneFrameResources.texture2Ds);
    Updater(updating.oneFrameResources.readWriteTexture2Ds,
             staging.oneFrameResources.readWriteTexture2Ds);

    Updater(updating.oneFrameResources.colorOutputs,
             staging.oneFrameResources.colorOutputs);
    Updater(updating.oneFrameResources.depthStencilOutputs,
             staging.oneFrameResources.depthStencilOutputs);
    Updater(updating.oneFrameResources.swapchainOutputs,
             staging.oneFrameResources.swapchainOutputs);
}

Resources& RasterizePass::AcquireFrameResource(unsigned int bufferingIndex)
{
    if (bufferingIndex < (frameResources.size() - 1)) {
        return frameResources[bufferingIndex];
    }
    TI_LOG_F(TAG, "Acquire frame resource out of range! "
                  "Return the staging frame resource instead.");
    return frameResources.back();
}

Resources& RasterizePass::AcquireStagingFrameResource()
{
    return frameResources.back();
}

void RasterizePass::ClearFrameResources()
{
    frameResources.clear(); // Clear all contents first.
    frameResources.resize(static_cast<size_t>(passflow.GetMultipleBufferingCount()) + 1);
}

}
