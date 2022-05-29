#pragma once

#include "common/TypeCast.hpp"

namespace ti::backend {
enum class BasicFormat {
    R8G8B8A8_UNORM,
    R32G32B32_FLOAT,
    R32G32B32A32_FLOAT,
    D24_UNORM_S8_UINT
    // ...
};

enum class IndexFormat {
    UINT16,
    UINT32
};

enum class VertexFormat {
    FLOAT32x3,
    FLOAT32x4
    // ...
};

enum class VertexInputRate {
    PER_VERTEX,
    PER_INSTANCE
};

enum class IndexStripCutValue {
    NONE_OR_DISABLE,
    UINT16_MAX_VALUE,
    UINT32_MAX_VALUE

};

enum class PrimitiveTopology {
    POINT_LIST,
    LINE_LIST,
    LINE_STRIP,
    TRIANGLE_LIST,
    TRIANGLE_STRIP
};

enum class TransferDirection {
    GPU_ONLY,
    CPU_TO_GPU,
    GPU_TO_CPU
};

enum class ResourceState {
    UNDEFINED,
    PRESENT,
    COLOR_OUTPUT,
    GENERAL_READ,
    DEPTH_STENCIL_READ,
    DEPTH_STENCIL_WRITE,
    COPY_SOURCE,
    COPY_DESTINATION,
    RESOLVE_SOURCE,
    RESOLVE_DESTINATION
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

enum class ImageType : uint8_t {
    Color   = 0x1,                  // render target / color attachment
    Depth   = 0x2,                  // depth, a part of depth stencil attachment
    Stencil = 0x4,                  // stencil, a part of depth stencil attachment
    DepthStencil = Depth | Stencil, // depth stencil / depth stencil attachment
    ShaderResource = 0x0            // generic image, cannot be used as attachment
};

enum class ImageDimension : uint8_t {
    Dimension1D = 1, // Texture 1D
    Dimension2D = 2, // Texture 2D
    Dimension3D = 3  // Texture 3D
};

enum class DescriptorType : uint8_t {
    ConstantBuffer = (1 << 0), // descriptor for constant buffer
    StorageBuffer  = (1 << 1), // descriptor for storage buffer
    GenericBuffer  = ConstantBuffer | StorageBuffer,
    ImageSampler   = (1 << 4), // descriptor for image sampler
    ColorOutput    = (1 << 5), // descriptor for render target
    DepthStencil   = (1 << 6)  // descriptor for depth stencil
};

enum class Stage : uint32_t {
    // Sort them by their order in the graphics pipeline:
    InputAssembler = (1 <<  0), // Standard stage: input vertex and index.
    VertexShader   = (1 <<  2), // Standard stage: vertex process.
    HullShader     = (1 <<  4), // Extra stage: hull process, determining how much an input control
                                // patch should be tessellated by the tessellation stage.
    Tessellation   = (1 <<  8), // Extra stage: tessellation, which converts low-detail subdivision
                                // surfaces into higher-detail primitives on the GPU.
    DomainShader   = (1 << 10), // Extra stage: domain process, compute the final vertex attributes
                                // based on the output control points from the hull shader and the
                                // interpolation coordinates from the tessellation stage.
    GeometryShader = (1 << 12), // Extra stage: geometry process, which runs shader code with vert-
                                // ices as input and the ability to generate vertices on output.
    StreamOutput   = (1 << 14), // Extra stage: output vertex data from the geometry-shader stage
                                // (or the vertex-shader stage if the geometry-shader stage is
                                // inactive) to one or more buffers in memory.
    Rasterizer     = (1 << 16), // Standard stage: rasterize.
    PixelShader    = (1 << 18), // Standard stage: pixel/fragment process.
    OutputMerger   = (1 << 20), // Standard stage: merge and output.
    ComputeShader  = (1 << 24), // Advance stage: compute pipeline process.
    // Programmable shader stage mask:
    ShaderStageMask = VertexShader | HullShader | DomainShader | GeometryShader |
                      PixelShader  | ComputeShader
};

enum class ShaderStage : uint32_t {
    Vertex   = common::EnumCast(Stage::VertexShader),
    Hull     = common::EnumCast(Stage::HullShader),
    Domain   = common::EnumCast(Stage::DomainShader),
    Geometry = common::EnumCast(Stage::GeometryShader),
    Pixel    = common::EnumCast(Stage::PixelShader),
    Compute  = common::EnumCast(Stage::ComputeShader),
    Graphics = Vertex | Hull | Domain | Geometry | Pixel
};

struct ClearValue {
    float color[4]{};
    float depth = 1.0f;
    uint8_t stencil = 0;
};

struct Viewport {
    float x        = 0.0f; // left top x
    float y        = 0.0f; // left top y
    float width    = 0.0f; // right bottom x = x + width
    float height   = 0.0f; // right bottom y = y + height
    float minDepth = 0.0f; // depth mapping to:
    float maxDepth = 1.0f; // [minDepth, maxDepth]
};

struct Scissor {
    long left   = 0;
    long top    = 0;
    long right  = 0;
    long bottom = 0;
};

enum class FillMode {
    Solid,
    Wireframe
};

enum class CullMode {
    None,
    Front,
    Back
};

struct RasterizerState {
    FillMode fillMode;
    CullMode cullMode;
};

inline bool IsBasicFormatHasDepth(BasicFormat format)
{
    switch (format) {
    case BasicFormat::D24_UNORM_S8_UINT:
        return true;
    }
    return false;
}

inline bool IsBasicFormatHasStencil(BasicFormat format)
{
    switch (format) {
    case BasicFormat::D24_UNORM_S8_UINT:
        return true;
    }
    return false;
}
}
