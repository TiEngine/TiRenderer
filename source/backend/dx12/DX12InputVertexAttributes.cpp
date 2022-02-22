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
    elementDesc.Format = ConvertFormat(attribute.format);
    elementDesc.InputSlot = attribute.slot;
    elementDesc.AlignedByteOffset = attribute.stride;
    elementDesc.InstanceDataStepRate = 0;
    if (attribute.slotClass == Attribute::SlotClass::PerVertexData) {
        elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    } else {
        elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
    }
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
}
