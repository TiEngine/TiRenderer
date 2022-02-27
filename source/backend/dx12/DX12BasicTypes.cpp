#include "DX12BasicTypes.h"
#include <unordered_map>

namespace ti::backend {
DXGI_FORMAT ConvertFormat(Format format)
{
    static const std::unordered_map<Format, DXGI_FORMAT> mapper = {
        { Format::R8G8B8A8_UNORM,     DXGI_FORMAT_R8G8B8A8_UNORM     },
        { Format::R32G32B32_FLOAT,    DXGI_FORMAT_R32G32B32_FLOAT    },
        { Format::R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
        { Format::D24_UNORM_S8_UINT,  DXGI_FORMAT_D24_UNORM_S8_UINT  }
    };
    return mapper.at(format);
}

unsigned int ConvertMSAA(MSAA msaa)
{
    return common::EnumCast(msaa);
}

D3D12_HEAP_TYPE ConvertHeap(TransferDirection type)
{
    static const std::unordered_map<TransferDirection, D3D12_HEAP_TYPE> mapper = {
        { TransferDirection::GPU_ONLY,   D3D12_HEAP_TYPE_DEFAULT  },
        { TransferDirection::CPU_TO_GPU, D3D12_HEAP_TYPE_UPLOAD   },
        { TransferDirection::GPU_TO_CPU, D3D12_HEAP_TYPE_READBACK }
    };
    return mapper.at(type);
}

D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state)
{
    static const std::unordered_map<ResourceState, D3D12_RESOURCE_STATES> mapper = {
        { ResourceState::UNDEFINED,           D3D12_RESOURCE_STATE_COMMON         },
        { ResourceState::PRESENT,             D3D12_RESOURCE_STATE_PRESENT        },
        { ResourceState::GENERAL_READ,        D3D12_RESOURCE_STATE_GENERIC_READ   },
        { ResourceState::DEPTH_STENCIL_READ,  D3D12_RESOURCE_STATE_DEPTH_READ     },
        { ResourceState::DEPTH_STENCIL_WRITE, D3D12_RESOURCE_STATE_DEPTH_WRITE    },
        { ResourceState::COPY_SOURCE,         D3D12_RESOURCE_STATE_COPY_SOURCE    },
        { ResourceState::COPY_DESTINATION,    D3D12_RESOURCE_STATE_COPY_DEST      },
        { ResourceState::RESOLVE_SOURCE,      D3D12_RESOURCE_STATE_RESOLVE_SOURCE },
        { ResourceState::RESOLVE_DESTINATION, D3D12_RESOURCE_STATE_RESOLVE_DEST   }
    };
    return mapper.at(state);
}

D3D12_CLEAR_VALUE ConvertClearValue(ClearValue value)
{
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = ConvertFormat(value.format);
    clearValue.Color[0] = value.color[0];
    clearValue.Color[1] = value.color[1];
    clearValue.Color[2] = value.color[2];
    clearValue.Color[3] = value.color[3];
    clearValue.DepthStencil.Depth = value.depth;
    clearValue.DepthStencil.Stencil = value.stencil;
    return clearValue;
}
}
