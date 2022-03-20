#pragma once

#include "backend/BasicTypes.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
DXGI_FORMAT ConvertBasicFormat(BasicFormat format);

DXGI_FORMAT ConvertIndexFormat(IndexFormat format);

DXGI_FORMAT ConvertVertexFormat(VertexFormat format);

D3D12_INPUT_CLASSIFICATION ConvertInputClassification(VertexInputRate rate);

D3D12_HEAP_TYPE ConvertHeap(TransferDirection type);

D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state);

unsigned int ConvertMSAA(MSAA msaa);

D3D12_RESOURCE_FLAGS ConvertImageResourceFlag(ImageType type);

D3D12_RESOURCE_DIMENSION ConvertImageDimension(ImageDimension dimension);

D3D12_DESCRIPTOR_HEAP_TYPE ConvertDescriptorHeap(DescriptorType type);

D3D12_CLEAR_VALUE ConvertClearValue(BasicFormat format, ClearValue value);
}
