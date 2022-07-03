#include "passflow/RasterizePass.h"

namespace ti::passflow {

RasterizePass::RasterizePass(Passflow& passflow) : BasePass(passflow)
{
}

RasterizePass::~RasterizePass()
{
    CleanupPipeline();
}

void RasterizePass::AddDrawItem(std::shared_ptr<DrawItem> item)
{
    stagingDrawItems.emplace_back(item);
}

void RasterizePass::CleanupPipeline()
{
    if (device) {
        if (inputVertexAttributes) {
            device->DestroyInputVertexAttributes(inputVertexAttributes);
        }
        if (inputIndexAttribute) {
            device->DestroyInputIndexAttribute(inputIndexAttribute);
        }

        for (const auto& shader : programShaders) {
            device->DestroyShader(shader.second);
        }

        for (const auto& group : descriptorGroups) {
            device->DestroyDescriptorGroup(group.second);
        }

        if (pipelineLayout) {
            device->DestroyPipelineLayout(pipelineLayout);
        }
        if (pipelineState) {
            device->DestroyPipelineState(pipelineState);
        }
    }
}

RasterizePass& RasterizePass::InitializePipeline(backend::Device* device)
{
    this->device = device;
    pipelineState = device->CreatePipelineState();
    pipelineLayout = device->CreatePipelineLayout();
    return *this;
}

RasterizePass& RasterizePass::DeclareInput(const InputProperties& properties)
{
    inputVertexAttributes = device->CreateInputVertexAttributes();
    for (const auto& attribute : properties.vertexAttributes) {
        inputVertexAttributes->AddAttribute(attribute);
    }
    inputIndexAttribute = device->CreateInputIndexAttribute();
    inputIndexAttribute->SetAttribute(properties.indexAttribute);
    return *this;
}

RasterizePass& RasterizePass::DeclareOutput(const OutputProperties& properties)
{
    for (const auto& target : properties.targets) {
        if (target.first == OutputProperties::OutputSlot::DS) {
            pipelineState->SetDepthStencilOutputFormat(target.second.imagePixelFormat);
        } else { // Color: C0,...,C7
            pipelineState->SetColorOutputFormat(
                common::EnumCast(target.first), target.second.imagePixelFormat);
        }
    }
    return *this;
}

RasterizePass& RasterizePass::DeclareProgram(const ProgramProperties& properties)
{
    for (const auto& shader : properties.shaders) {
        programShaders[shader.first] = device->CreateShader({ shader.first, shader.second });
    }
    return *this;
}

RasterizePass& RasterizePass::DeclareResource(const ShaderResourceProperties& properties)
{
    pipelineLayout = device->CreatePipelineLayout();
    for (const auto& resources : properties.resources) {
        auto space = common::EnumCast(resources.first);
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& resource : resources.second) {
            if (resource.bindingPointCount == 1) {
                group->AddDescriptor(
                    resource.resourceType,
                    resource.baseBindingPoint,
                    resource.resourceVisibility);
            } else if (resource.bindingPointCount > 1) {
                unsigned int begBp = resource.baseBindingPoint;
                unsigned int endBp = resource.baseBindingPoint +
                                     resource.bindingPointCount - 1;
                group->AddDescriptors(
                    resource.resourceType,
                    std::make_pair(begBp, endBp),
                    resource.resourceVisibility);
            }
        }
        pipelineLayout->AddGroup(group);
    }
    pipelineLayout->BuildLayout();
    return *this;
}

void RasterizePass::BuildPipeline()
{
    pipelineState->SetPipelineLayout(pipelineLayout);
    pipelineState->SetIndexAssembly(inputIndexAttribute);
    pipelineState->SetVertexAssembly(inputVertexAttributes);
    for (const auto& shader : programShaders) {
        pipelineState->SetShader(shader.first, shader.second);
    }
    pipelineState->BuildState();
}

}
