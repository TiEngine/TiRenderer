#include "DX12BasicTypes.h"
#include <unordered_map>

namespace ti::backend {
DXGI_FORMAT ConvertBasicFormat(BasicFormat format)
{
    static const std::unordered_map<BasicFormat, DXGI_FORMAT> map = {
        { BasicFormat::R8G8B8A8_UNORM,     DXGI_FORMAT_R8G8B8A8_UNORM     },
        { BasicFormat::R32G32B32_FLOAT,    DXGI_FORMAT_R32G32B32_FLOAT    },
        { BasicFormat::R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
        { BasicFormat::D24_UNORM_S8_UINT,  DXGI_FORMAT_D24_UNORM_S8_UINT  }
    };
    return map.at(format);
}

DXGI_FORMAT ConvertIndexFormat(IndexFormat format)
{
    static const std::unordered_map<IndexFormat, DXGI_FORMAT> map = {
        { IndexFormat::UINT16, DXGI_FORMAT_R16_UINT },
        { IndexFormat::UINT32, DXGI_FORMAT_R32_UINT }
    };
    return map.at(format);
}

DXGI_FORMAT ConvertVertexFormat(VertexFormat format)
{
    static const std::unordered_map<VertexFormat, DXGI_FORMAT> map = {
        { VertexFormat::FLOAT32x3, DXGI_FORMAT_R32G32B32_FLOAT    },
        { VertexFormat::FLOAT32x4, DXGI_FORMAT_R32G32B32A32_FLOAT }
    };
    return map.at(format);
}

D3D12_INPUT_CLASSIFICATION ConvertInputClassification(VertexInputRate rate)
{
    static const std::unordered_map<VertexInputRate, D3D12_INPUT_CLASSIFICATION> map = {
        { VertexInputRate::PER_VERTEX,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA   },
        { VertexInputRate::PER_INSTANCE, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA }
    };
    return map.at(rate);
}

D3D12_HEAP_TYPE ConvertHeap(TransferDirection type)
{
    static const std::unordered_map<TransferDirection, D3D12_HEAP_TYPE> map = {
        { TransferDirection::GPU_ONLY,   D3D12_HEAP_TYPE_DEFAULT  },
        { TransferDirection::CPU_TO_GPU, D3D12_HEAP_TYPE_UPLOAD   },
        { TransferDirection::GPU_TO_CPU, D3D12_HEAP_TYPE_READBACK }
    };
    return map.at(type);
}

D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state)
{
    static const std::unordered_map<ResourceState, D3D12_RESOURCE_STATES> map = {
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
    return map.at(state);
}

unsigned int ConvertMSAA(MSAA msaa)
{
    return common::EnumCast(msaa);
}

D3D12_RESOURCE_FLAGS ConvertImageResourceFlag(ImageType type)
{
    static const std::unordered_map<ImageType, D3D12_RESOURCE_FLAGS> map = {
        { ImageType::Color,          D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET },
        { ImageType::Depth,          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        { ImageType::Stencil,        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        { ImageType::DepthStencil,   D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        { ImageType::ShaderResource, D3D12_RESOURCE_FLAG_NONE                }
    };
    return map.at(type);
}

D3D12_RESOURCE_DIMENSION ConvertImageDimension(ImageDimension dimension)
{
    static const std::unordered_map<ImageDimension, D3D12_RESOURCE_DIMENSION> map = {
        { ImageDimension::Dimension1D, D3D12_RESOURCE_DIMENSION_TEXTURE1D },
        { ImageDimension::Dimension2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D },
        { ImageDimension::Dimension3D, D3D12_RESOURCE_DIMENSION_TEXTURE3D }
    };
    return map.at(dimension);
}

D3D12_CLEAR_VALUE ConvertClearValue(BasicFormat format, ClearValue value)
{
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = ConvertBasicFormat(format);
    clearValue.Color[0] = value.color[0];
    clearValue.Color[1] = value.color[1];
    clearValue.Color[2] = value.color[2];
    clearValue.Color[3] = value.color[3];
    clearValue.DepthStencil.Depth = value.depth;
    clearValue.DepthStencil.Stencil = value.stencil;
    return clearValue;
}
}
