#include "DX12DescriptorGroup.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace ti::backend {
DX12DescriptorGroup::DX12DescriptorGroup(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12DescriptorGroup::~DX12DescriptorGroup()
{
    Shutdown();
}

void DX12DescriptorGroup::Setup(Description description)
{
    this->description = description;
}

void DX12DescriptorGroup::Shutdown()
{
    description = { 0u };
    descriptorCounter = 0;
    parameters.clear();
    descriptorRanges.clear();
}

void DX12DescriptorGroup::AddDescriptor(DescriptorType type,
    unsigned int id, unsigned int space, ShaderStage visibility)
{
    CD3DX12_ROOT_PARAMETER parameter{};

    switch (type) {
    case DescriptorType::ConstantBuffer:
        parameter.InitAsConstantBufferView(id, space, ConvertShaderVisibility(visibility));
        break;

    case DescriptorType::StorageBuffer:
        // TODO
        break;

    case DescriptorType::ImageSampler:
        // TODO
        break;

    default:
        TI_LOG_RET_F(TAG, "Add descriptor information to descriptor group failed! "
            "Invalid descriptor type, only can be ConstantBuffer/StorageBuffer/ImageSampler.");
    }

    if (descriptorCounter + 1 > description.descriptorCount) {
        TI_LOG_W(TAG, "The number of descriptors overflowed, add descriptor information "
            "to descriptor group success but you better check your logic and code!");
    }
    descriptorCounter++;

    parameters.emplace_back(parameter);
}

void DX12DescriptorGroup::AddDescriptors(DescriptorType type,
    unsigned int beginId, unsigned int endId, unsigned int space, ShaderStage visibility)
{
    if (beginId > endId) {
        TI_LOG_RET_F(TAG, "Add descriptor information to descriptor group failed! "
            "Arguments are invalid, failed because the begin id is large then the end id.");
    }

    CD3DX12_ROOT_PARAMETER parameter{};

    switch (type) {
    case DescriptorType::ConstantBuffer:
        descriptorRanges.emplace_back(std::make_unique<CD3DX12_DESCRIPTOR_RANGE>());
        descriptorRanges.back()->Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            endId - beginId, beginId, space);
        parameter.InitAsDescriptorTable(1, descriptorRanges.back().get(),
            ConvertShaderVisibility(visibility));
        break;

    case DescriptorType::StorageBuffer:
        // TODO
        break;

    case DescriptorType::ImageSampler:
        // TODO
        break;

    default:
        TI_LOG_RET_F(TAG, "Add ranged descriptors information to descriptor group failed! "
            "Invalid descriptor type, only can be ConstantBuffer/StorageBuffer/ImageSampler.");
    }

    if (descriptorCounter + (endId - beginId) > description.descriptorCount) {
        TI_LOG_W(TAG, "The number of descriptors overflowed, add ranged descriptors information "
            "to descriptor group success but you better check your logic and code!");
    }
    descriptorCounter += endId - beginId;

    parameters.emplace_back(parameter);
}
}
