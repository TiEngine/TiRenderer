#pragma once

#include "common/TypeCast.hpp"

namespace ti::backend {
enum class Format {
    R8G8B8A8_UNORM,
    R32G32B32_FLOAT,
    R32G32B32A32_FLOAT,
    D24_UNORM_S8_UINT
};

enum class Stage : uint32_t {
    // Sort them by their order in the graphics pipeline:
    InputAssembler = (1 <<  0), // Standard stage: input vertex and index.
    VertexShader   = (1 <<  2), // Standard stage: vertex process.
    HullShader     = (1 <<  4), // Extra stage: hull process, determining how much an input control
                                // patch should be tessellated by the tessellation stage.
    Tessellation   = (1 <<  8), // Extra stage: tessellation, which converts low-detail subdivision
                                // surfaces into higher-detail primitives on the GPU.
    DomainShader   = (1 << 10), // Extra stage: domain process, computes the final vertex attributes
                                // based on the output control points from the hull shader and the
                                // interpolation coordinates from the tessellation stage.
    GeometryShader = (1 << 12), // Extra stage: geometry process, which runs shader code with vertices
                                // as input and the ability to generate vertices on output.
    StreamOutput   = (1 << 14), // Extra stage: output vertex data from the geometry-shader stage (or
                                // the vertex-shader stage if the geometry-shader stage is inactive)
                                // to one or more buffers in memory.
    Rasterizer     = (1 << 16), // Standard stage: rasterize.
    PixelShader    = (1 << 18), // Standard stage: pixel/fragment process.
    OutputMerger   = (1 << 20), // Standard stage: merge and output.
    ComputeShader  = (1 << 24), // Advance stage: compute pipeline process.
    // Programmable shader stage mask:
    ShaderStageMask = VertexShader | HullShader | DomainShader | GeometryShader | PixelShader | ComputeShader
};

enum class ShaderStage : uint32_t {
    Vertex   = common::EnumCast(Stage::VertexShader),
    Hull     = common::EnumCast(Stage::HullShader),
    Domain   = common::EnumCast(Stage::DomainShader),
    Geometry = common::EnumCast(Stage::GeometryShader),
    Pixel    = common::EnumCast(Stage::PixelShader),
    Compute  = common::EnumCast(Stage::ComputeShader)
};

enum class MSAA : uint8_t {
    MSAAx1 = 1,
    MSAAx4 = 4
};

enum class CommandType : uint8_t {
    Graphics = 0x1,
    Transfer = 0x2,
    Compute  = 0x4,
    Generic  = Graphics | Transfer | Compute
};

enum class TransferDirection {
    GPU_ONLY,
    CPU_TO_GPU,
    GPU_TO_CPU
};

enum class ResourceState {
    UNDEFINED,
    PRESENT,
    GENERAL_READ,
    DEPTH_STENCIL_READ,
    DEPTH_STENCIL_WRITE,
    COPY_SOURCE,
    COPY_DESTINATION,
    RESOLVE_SOURCE,
    RESOLVE_DESTINATION
};

struct ColorValue {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

struct ClearValue {
    Format format = Format::R32G32B32A32_FLOAT;
    float color[4]{};
    float depth = 1.0f;
    uint8_t stencil = 0;
};

struct ScreenPoint {
    float x = 0.0f;
    float y = 0.0f;
};

struct ScreenRectangle {
    ScreenPoint leftTop;
    ScreenPoint rightBottom;
};
}
