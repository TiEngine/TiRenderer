#include "passflow/BasePass.h"
#include "../general/Backend.h"

namespace ti::passflow {

BasePass::BasePass(Passflow& passflow) : passflow(passflow)
{
}

backend::InputVertexAttributes::Attribute
BasePass::InputProperties::MakeDefaultPositionOnlyVertexAttribute()
{
    return { VertexFormat::FLOAT32x3, "POSITION", 0, 0 };
}

BasePass::DynamicDescriptorManager::DynamicDescriptorManager(backend::Device* device)
{
    bkDevice = Backend::GetReference().Device();
}

BasePass::DynamicDescriptorManager::~DynamicDescriptorManager()
{
}

void BasePass::DynamicDescriptorManager::ReallocateDescriptorHeap(unsigned int descriptorCount)
{
}

backend::Descriptor* BasePass::DynamicDescriptorManager::AcquireDescriptor(unsigned int index)
{
    return nullptr;
}

void BasePass::DynamicDescriptorManager::FreeDescriptorHeap()
{
}

}
