#include "passflow/RasterizePass.h"
#include "passflow/Passflow.h"

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

void RasterizePass::InitializePipeline(backend::Device* device)
{
    this->device = device;

    pipelineState = device->CreatePipelineState();
    pipelineLayout = device->CreatePipelineLayout();

    unsigned int mbc = passflow.GetMultipleBufferingCount();
    shaderResourceDescriptorHeaps.resize(mbc, { device, DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(mbc, { device, DescriptorType::ImageSampler });
    renderTargetDescriptorHeaps.resize(mbc, { device, DescriptorType::ShaderResource });
    depthStencilDescriptorHeaps.resize(mbc, { device, DescriptorType::ShaderResource });
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
        // If the number of objects added by calling AddDrawItem exceeds it,
        // it will be doubled when the reserved descriptor heap is not enough.
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
    pipelineLayout = device->CreatePipelineLayout();
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = common::EnumCast(resourceSpace);
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& attribute : resourceAttributes) {
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

    for (unsigned int index = 0; index < passflow.GetMultipleBufferingCount(); index++) {
        shaderResourceDescriptorHeaps[index].ReallocateDescriptorHeap();
        imageSamplerDescriptorHeaps[index].ReallocateDescriptorHeap();
        renderTargetDescriptorHeaps[index].ReallocateDescriptorHeap(
            rasterizePipelineCounters.colorOutputCount);
        depthStencilDescriptorHeaps[index].ReallocateDescriptorHeap(
            rasterizePipelineCounters.depthStencilOutputCount);

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

        // Not owned the device, so set it to null simply.
        device = nullptr;

        // Reset recorded counters variable to default values.
        rasterizePipelineCounters = {};
    }
}

}
