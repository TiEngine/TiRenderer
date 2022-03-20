#include "DemosBaseHeader.h"
#include <vector>

const std::string Demo_01_Backend::vertexShaderString = R"(
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

const std::string Demo_01_Backend::fragmentShaderString = R"(
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

const std::vector<Demo_01_Backend::VertexData> Demo_01_Backend::vertices = {
    { ti::math::XMFLOAT3(-1.0f, -1.0f, -1.0f),
      ti::math::XMFLOAT4(1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f) }, // White
    { ti::math::XMFLOAT3(-1.0f, +1.0f, -1.0f),
      ti::math::XMFLOAT4(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f) }, // Black
    { ti::math::XMFLOAT3(+1.0f, +1.0f, -1.0f),
      ti::math::XMFLOAT4(1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f) }, // Red
    { ti::math::XMFLOAT3(+1.0f, -1.0f, -1.0f),
      ti::math::XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f) }, // Green
    { ti::math::XMFLOAT3(-1.0f, -1.0f, +1.0f),
      ti::math::XMFLOAT4(0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f) }, // Blue
    { ti::math::XMFLOAT3(-1.0f, +1.0f, +1.0f),
      ti::math::XMFLOAT4(1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f) }, // Yellow
    { ti::math::XMFLOAT3(+1.0f, +1.0f, +1.0f),
      ti::math::XMFLOAT4(0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f) }, // Cyan
    { ti::math::XMFLOAT3(+1.0f, -1.0f, +1.0f),
      ti::math::XMFLOAT4(1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f) }  // Magenta
};

const std::vector<uint16_t> Demo_01_Backend::indices = {
    // front face
    0, 1, 2,
    0, 2, 3,
    // back face
    4, 6, 5,
    4, 7, 6,
    // left face
    4, 5, 1,
    4, 1, 0,
    // right face
    3, 2, 6,
    3, 6, 7,
    // top face
    1, 5, 6,
    1, 6, 2,
    // bottom face
    4, 0, 3,
    4, 3, 7
};

void Demo_01_Backend::Begin()
{
    backend = ti::backend::BackendContext::CreateBackend(
        ti::backend::BackendContext::Backend::DX12);
    device = backend->CreateDevice({});

    commandRecorder = device->CreateCommandRecorder({});

    vertexShader = device->CreateShader({ ti::backend::ShaderStage::Vertex, vertexShaderString });
    pixelShader = device->CreateShader({ ti::backend::ShaderStage::Pixel, fragmentShaderString });

    inputVertexAttributes = device->CreateInputVertexAttributes();
    inputVertexAttributes->AddAttribute(
        { ti::backend::VertexFormat::FLOAT32x3, "POSITION", 0,  0 });
    inputVertexAttributes->AddAttribute(
        { ti::backend::VertexFormat::FLOAT32x4, "COLOR", 1, 12 });

    vertexInput = device->CreateInputVertex({
        static_cast<unsigned int>(vertices.size()), sizeof(VertexData) });
    {   // Upload vertex buffer to GPU_ONLY buffer
        auto staging = device->CreateInputVertex({
            static_cast<unsigned int>(vertices.size()), sizeof(VertexData),
            ti::backend::TransferDirection::CPU_TO_GPU });
        auto transfer = device->CreateCommandRecorder({
            ti::backend::CommandType::Transfer });
        transfer->BeginRecord();
        transfer->RcBarrier(*vertexInput,
            ti::backend::ResourceState::GENERAL_READ,
            ti::backend::ResourceState::COPY_DESTINATION);
        transfer->RcUpload(*vertexInput, *staging,
            vertices.size() * sizeof(VertexData), static_cast<const void*>(vertices.data()));
        transfer->RcBarrier(*vertexInput,
            ti::backend::ResourceState::COPY_DESTINATION,
            ti::backend::ResourceState::GENERAL_READ);
        transfer->EndRecord();
        transfer->Submit();
        transfer->Wait();
    }

    indexInput = device->CreateInputIndex({
        static_cast<unsigned int>(indices.size()), sizeof(uint16_t) });
    {   // Upload index buffer to GPU_ONLY buffer
        auto staging = device->CreateInputIndex({
            static_cast<unsigned int>(indices.size()), sizeof(uint16_t),
            ti::backend::TransferDirection::CPU_TO_GPU });
        auto transfer = device->CreateCommandRecorder({
            ti::backend::CommandType::Transfer });
        transfer->BeginRecord();
        transfer->RcBarrier(*indexInput,
            ti::backend::ResourceState::GENERAL_READ,
            ti::backend::ResourceState::COPY_DESTINATION);
        transfer->RcUpload(*indexInput, *staging,
            indices.size() * sizeof(uint16_t), static_cast<const void*>(indices.data()));
        transfer->RcBarrier(*indexInput,
            ti::backend::ResourceState::COPY_DESTINATION,
            ti::backend::ResourceState::GENERAL_READ);
        transfer->EndRecord();
        transfer->Submit();
        transfer->Wait();
    }

    cbObjectMVP = device->CreateResourceBuffer({ sizeof(ObjectMVP) });
}

void Demo_01_Backend::Finish()
{
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
    if (updateObjectMVP) {
        device->WaitIdle();
        memcpy(cbObjectMVP->Map(), &objectMVP, sizeof(ObjectMVP));
        cbObjectMVP->Unmap();
    }
    //swapchain->Present();
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
