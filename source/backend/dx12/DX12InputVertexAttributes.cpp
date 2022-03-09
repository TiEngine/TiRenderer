#pragma once

#include "DX12InputVertexAttributes.h"
#include "DX12BasicTypes.h"

namespace ti::backend {
DX12InputVertexAttributes::DX12InputVertexAttributes()
{
}

DX12InputVertexAttributes::~DX12InputVertexAttributes()
{
    Shutdown();
}

void DX12InputVertexAttributes::Setup(Description description)
{
    inputLayout.reserve(description.reservedAttributesCount);
    semanticNames.reserve(description.reservedAttributesCount);
}

void DX12InputVertexAttributes::Shutdown()
{
    inputLayout.resize(0);
    semanticNames.resize(0);
}

void DX12InputVertexAttributes::AddAttribute(Attribute attribute)
{
    // NB: Make sure the semantic name is unique and the slot+location is unique.
    semanticNames.emplace_back(attribute.semantic);
    D3D12_INPUT_ELEMENT_DESC elementDesc{};
    elementDesc.SemanticName = semanticNames.back().c_str();
    elementDesc.SemanticIndex = 0;
    elementDesc.Format = ConvertBasicFormat(attribute.format);
    elementDesc.InputSlot = attribute.slot;
    elementDesc.InputSlotClass = SelectSlotClass(attribute.slotClass);
    elementDesc.AlignedByteOffset = attribute.stride;
    elementDesc.InstanceDataStepRate = 0;
    inputLayout.emplace_back(elementDesc);
    // NB: The change of the semanticNames container will invalidate the result of c_str()!
    for (size_t n = 0; n < semanticNames.size(); n++) {
        inputLayout[n].SemanticName = semanticNames[n].c_str();
    }
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& DX12InputVertexAttributes::GetInputLayout() const
{
    return inputLayout;
}

D3D12_INPUT_CLASSIFICATION DX12InputVertexAttributes::SelectSlotClass(Attribute::SlotClass type) const
{
    switch (type) {
    case Attribute::SlotClass::PerVertexData:
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    case Attribute::SlotClass::PerInstanceData:
        return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
    }
    return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
}
}
