#pragma once

#include "backend/BasicTypes.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {

DXGI_FORMAT ConvertFormat(Format format);

unsigned int ConvertMSAA(MSAA msaa);

D3D12_HEAP_TYPE ConvertHeap(TransferDirection type);

D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state);

D3D12_RESOURCE_FLAGS ConvertImageResourceFlag(ImageType type);

D3D12_CLEAR_VALUE ConvertClearValue(Format format, ClearValue value);

}
