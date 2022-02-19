#pragma once

#include <cstdint>

namespace ti::backend {

enum class Format {
    R8G8B8A8_UNORM,
    D24_UNORM_S8_UINT
};

enum class Stage : uint32_t {
    // Sort them by their order in the graphics pipeline:
    InputAssembler = (1 <<  0), // Standard stage: input vertex and index
    VertexShader   = (1 <<  2), // Standard stage: vertex process
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
    Rasterizer     = (1 << 16), // Standard stage: rasterize
    PixelShader    = (1 << 18), // Standard stage: pixel/fragment process
    OutputMerger   = (1 << 20), // Standard stage: merge and output
    ComputeShader  = (1 << 24), // Advance stage: compute pipeline process
    // Programmable shader stage mask:
    ShaderStageMask = VertexShader | HullShader | DomainShader | GeometryShader | PixelShader | ComputeShader
};

enum class ShaderStage : uint32_t {
    None     = 0, // default none stage
    Vertex   = static_cast<uint32_t>(Stage::VertexShader),
    Hull     = static_cast<uint32_t>(Stage::HullShader),
    Domain   = static_cast<uint32_t>(Stage::DomainShader),
    Geometry = static_cast<uint32_t>(Stage::GeometryShader),
    Pixel    = static_cast<uint32_t>(Stage::PixelShader),
    Compute  = static_cast<uint32_t>(Stage::ComputeShader)
};

enum class MSAA : uint8_t {
    MSAAx1 = 1,
    MSAAx4 = 4
};

}
