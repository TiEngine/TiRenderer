#include "passflow/RasterizePass.h"
#include "passflow/Passflow.h"

namespace {

LOG_TAG(RasterizePass)

}

namespace ti::passflow {

RasterizePass::RasterizePass(Passflow& passflow) : BasePass(passflow)
{
}

RasterizePass::~RasterizePass()
{
    CleanPipeline();
}

void RasterizePass::AddDrawItem(std::shared_ptr<DrawItem> item)
{
    stagingDrawItems.emplace_back(item);
}

void RasterizePass::AddDrawItems(std::vector<std::shared_ptr<DrawItem>> items)
{
    stagingDrawItems.insert(stagingDrawItems.end(), items.begin(), items.end()); // Append.
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
        if (rasterizePipelineCounters.reservedObjectsCount >= stagingDrawItems.size()) {
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

}
