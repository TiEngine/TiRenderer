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

struct VertexData {
    ti::math::XMFLOAT3 position;
    ti::math::XMFLOAT4 color;
};

const std::vector<VertexData> vertices = {
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

const std::vector<uint16_t> indices = {
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

struct ObjectMVP {
    ti::math::XMFLOAT4X4 mvp =
        ti::math::XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
} objectMVP;

void Demo_01_Backend::Begin()
{
    backend = ti::backend::BackendContext::CreateBackend(ti::backend::BackendContext::Backend::DX12);
    device = backend->CreateDevice({});

    commandRecorder = device->CreateCommandRecorder({});

    vertexShader = device->CreateShader({ ti::backend::ShaderStage::Vertex, VertexShaderString });
    pixelShader = device->CreateShader({ ti::backend::ShaderStage::Pixel, FragmentShaderString });

    inputVertexAttributes = device->CreateInputVertexAttributes({});
    inputVertexAttributes->AddAttribute({ ti::backend::BasicFormat::R32G32B32_FLOAT,    "POSITION", 0,  0 });
    inputVertexAttributes->AddAttribute({ ti::backend::BasicFormat::R32G32B32A32_FLOAT, "COLOR",    1, 12 });

    std::vector<uint8_t> vertexUploadBuffer;
    vertexUploadBuffer.resize(vertices.size() * sizeof(VertexData));
    for (size_t n = 0; n < vertices.size(); n++) {
        new (&(vertexUploadBuffer[n * sizeof(VertexData)])) VertexData(vertices[n]);
    }
    vertexInput = device->CreateInputVertex({
        static_cast<unsigned int>(vertices.size()), sizeof(VertexData) });
    vertexInput->Upload(vertexUploadBuffer);

    std::vector<uint8_t> indexUploadBuffer;
    indexUploadBuffer.resize(indices.size() * sizeof(uint16_t));
    for (size_t n = 0; n < indices.size(); n++) {
        new (&(indexUploadBuffer[n * sizeof(uint16_t)])) uint16_t(indices[n]);
    }
    indexInput = device->CreateInputIndex({
        static_cast<unsigned int>(indices.size()), sizeof(uint16_t) });
    indexInput->Upload(indexUploadBuffer);

    cbObjectMVP = device->CreateResourceBuffer({ sizeof(ObjectMVP) });
}

void Demo_01_Backend::Finish()
{
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
    std::vector<uint8_t> mvpBuffer;
    mvpBuffer.resize(sizeof(ObjectMVP));
    new (mvpBuffer.data()) ObjectMVP(objectMVP);
    cbObjectMVP->Upload(mvpBuffer);
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
