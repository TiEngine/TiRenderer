#include "DemosBaseHeader.h"

const std::string VertexShaderString = R"(
cbuffer cbPerObject : register(b0)
{
    float4x4 gModelViewProj;
};

struct VertexIn
{
    float3 inLocalPos : POSITION;
    float4 inColor    : COLOR;
};

struct VertexOut
{
    float4 outWorldPos : SV_POSITION;
    float4 outColor    : COLOR;
};

VertexOut Main(VertexIn vin)
{
    VertexOut vout;
    // Transform to homogeneous clip space.
    vout.outWorldPos = mul(float4(vin.inLocalPos, 1.0f), gModelViewProj);
    // Just pass vertex color into the pixel shader.
    vout.outColor = vin.inColor;
    // Output to next pass.
    return vout;
}
)";

const std::string FragmentShaderString = R"(
struct VertexOut
{
    float4 outWorldPos : SV_POSITION;
    float4 outColor    : COLOR;
};

struct PixelOut
{
    float4 color : SV_Target;
};

PixelOut Main(VertexOut pin)
{
    PixelOut pout;
    pout.color = pin.outColor;
    return pout;
}
)";

void Demo_01_Backend::Begin()
{
    backend = ti::backend::BackendContext::CreateBackend(ti::backend::BackendContext::Backend::DX12);
    device = backend->CreateDevice({});

    commandAllocator = device->CreateCommandAllocator({});
    commandList = commandAllocator->CreateCommandList({});

    vertexShader = device->CreateShader({ ti::backend::ShaderStage::Vertex, VertexShaderString });
    pixelShader = device->CreateShader({ ti::backend::ShaderStage::Pixel, FragmentShaderString });

    inputVertexAttributes = device->CreateInputVertexAttributes({});
    inputVertexAttributes->AddAttribute({ ti::backend::Format::R32G32B32_FLOAT,    "POSITION", 0,  0 });
    inputVertexAttributes->AddAttribute({ ti::backend::Format::R32G32B32A32_FLOAT, "COLOR",    1, 12 });
}

void Demo_01_Backend::Finish()
{
    if (swapchain) {
        device->DestroySwapchain(swapchain);
    }

    commandAllocator->DestroyCommandList(commandList);
    device->DestroyCommandAllocator(commandAllocator);

    backend->DestroyDevice(device);
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
}

void Demo_01_Backend::Resize(HWND window, unsigned int width, unsigned int height)
{
    if (swapchain) {
        device->WaitIdle();
        swapchain->Resize(width, height);
    } else {
        swapchain = device->CreateSwapchain({ window, width, height });
    }
}
