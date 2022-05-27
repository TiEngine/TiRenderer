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

    commandRecorder = device->CreateCommandRecorder({ "DrawToSwapchain" });

    vertexShader = device->CreateShader({
        ti::backend::ShaderStage::Vertex, vertexShaderString });
    pixelShader = device->CreateShader({
        ti::backend::ShaderStage::Pixel, fragmentShaderString });

    inputVertexAttributes = device->CreateInputVertexAttributes();
    inputVertexAttributes->AddAttribute({
        ti::backend::VertexFormat::FLOAT32x3, "POSITION", 0,  0 });
    inputVertexAttributes->AddAttribute({
        ti::backend::VertexFormat::FLOAT32x4, "COLOR", 1, 12 });

    inputIndexAttribute = device->CreateInputIndexAttribute();
    inputIndexAttribute->SetAttribute({
        ti::backend::IndexFormat::UINT16, ti::backend::PrimitiveTopology::TRIANGLE_LIST });

    inputVertex = device->CreateInputVertex({
        static_cast<unsigned int>(vertices.size()), sizeof(VertexData) });
    {   // Upload vertex buffer to GPU_ONLY buffer
        auto staging = device->CreateInputVertex({
            static_cast<unsigned int>(vertices.size()), sizeof(VertexData),
            ti::backend::TransferDirection::CPU_TO_GPU });
        auto transfer = device->CreateCommandRecorder({
            "Transfer", ti::backend::CommandType::Transfer});
        transfer->BeginRecord();
        transfer->RcBarrier(inputVertex,
            ti::backend::ResourceState::GENERAL_READ,
            ti::backend::ResourceState::COPY_DESTINATION);
        transfer->RcUpload(static_cast<const void*>(vertices.data()),
            vertices.size() * sizeof(VertexData), inputVertex, staging);
        transfer->RcBarrier(inputVertex,
            ti::backend::ResourceState::COPY_DESTINATION,
            ti::backend::ResourceState::GENERAL_READ);
        transfer->EndRecord();
        transfer->Submit();
        transfer->Wait();
        device->RleaseCommandRecordersMemory("Transfer");
    }

    inputIndex = device->CreateInputIndex({
        static_cast<unsigned int>(indices.size()), sizeof(uint16_t) });
    {   // Upload index buffer to GPU_ONLY buffer
        auto staging = device->CreateInputIndex({
            static_cast<unsigned int>(indices.size()), sizeof(uint16_t),
            ti::backend::TransferDirection::CPU_TO_GPU });
        auto transfer = device->CreateCommandRecorder({
            "Transfer", ti::backend::CommandType::Transfer });
        transfer->BeginRecord();
        transfer->RcBarrier(inputIndex,
            ti::backend::ResourceState::GENERAL_READ,
            ti::backend::ResourceState::COPY_DESTINATION);
        transfer->RcUpload(static_cast<const void*>(indices.data()),
            indices.size() * sizeof(uint16_t), inputIndex, staging);
        transfer->RcBarrier(inputIndex,
            ti::backend::ResourceState::COPY_DESTINATION,
            ti::backend::ResourceState::GENERAL_READ);
        transfer->EndRecord();
        transfer->Submit();
        transfer->Wait();
        device->RleaseCommandRecordersMemory("Transfer");
    }

    descriptorHeap = device->CreateDescriptorHeap({
        1, ti::backend::DescriptorType::ConstantBuffer });

    cbObjectMVP = device->CreateResourceBuffer({ sizeof(ObjectMVP) });
    descriptorForObjectMVP = descriptorHeap->AllocateDescriptor({
        ti::backend::DescriptorType::ConstantBuffer });
    descriptorForObjectMVP->BuildDescriptor(cbObjectMVP);

    descriptorGroup = device->CreateDescriptorGroup({ 0 }); // space/binding is 0
    descriptorGroup->AddDescriptor(                         // descriptor 0, ConstantBuffer
        ti::backend::DescriptorType::ConstantBuffer,
        0, ti::backend::ShaderStage::Graphics);             // register/location is 0

    pipelineLayout = device->CreatePipelineLayout();
    pipelineLayout->AddGroup(descriptorGroup);              // this group has descriptor 0
    pipelineLayout->BuildLayout();

    pipelineState = device->CreatePipelineState();
    pipelineState->SetPipelineLayout(pipelineLayout);
    pipelineState->SetIndexAssembly(inputIndexAttribute);
    pipelineState->SetVertexAssembly(inputVertexAttributes);
    pipelineState->SetShader(ti::backend::ShaderStage::Vertex, vertexShader);
    pipelineState->SetShader(ti::backend::ShaderStage::Pixel, pixelShader);
    pipelineState->SetColorAttachment(0, ColorAttachmentFormat);
    pipelineState->SetDepthStencilAttachment(DepthStencilAttachmentFormat);
    pipelineState->BuildState();
}

void Demo_01_Backend::Finish()
{
    device->WaitIdle();
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
    frame++;
    AutomateRotate();
    if (updateObjectMVP) {
        device->WaitIdle();
        device->RleaseCommandRecordersMemory("DrawToSwapchain");
        memcpy(cbObjectMVP->Map(), &objectMVP, sizeof(ObjectMVP));
        cbObjectMVP->Unmap();
    }
    updateObjectMVP = false;
}

void Demo_01_Backend::Draw()
{
    commandRecorder->BeginRecord(pipelineState);

    commandRecorder->RcSetViewports({ viewport });
    commandRecorder->RcSetScissors({ scissor });

    commandRecorder->RcBarrier(swapchain,
        ti::backend::ResourceState::PRESENT, ti::backend::ResourceState::COLOR_OUTPUT);

    commandRecorder->RcClearColorAttachment(swapchain);
    commandRecorder->RcClearDepthStencilAttachment(swapchain);

    commandRecorder->RcSetRenderAttachments(swapchain, {}, {}, true);

    commandRecorder->RcSetDescriptorHeap({ descriptorHeap });

    commandRecorder->RcSetPipelineLayout(pipelineLayout);

    commandRecorder->RcSetVertex({ inputVertex }, inputVertexAttributes);
    commandRecorder->RcSetIndex(inputIndex, inputIndexAttribute);

    commandRecorder->RcSetDescriptor(0, cbObjectMVP); // descriptor 0, ConstantBuffer

    commandRecorder->RcDraw(inputIndex);

    commandRecorder->RcBarrier(swapchain,
        ti::backend::ResourceState::COLOR_OUTPUT, ti::backend::ResourceState::PRESENT);

    commandRecorder->EndRecord();

    commandRecorder->Submit();

    swapchain->Present();
}

void Demo_01_Backend::Resize(HWND window, unsigned int width, unsigned int height)
{
    if (swapchain) {
        device->WaitIdle();
        swapchain->Resize(width, height);
    } else {
        ti::backend::Swapchain::Description description{ window, width, height };
        swapchain = device->CreateSwapchain(description);
    }

    aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    viewport.width = static_cast<float>(width);
    viewport.height = static_cast<float>(height);

    scissor.right = width;
    scissor.bottom = height;
}

void Demo_01_Backend::AutomateRotate()
{
    theta += delta;

    // Convert Spherical to Cartesian coordinates.
    float x = radius * sinf(phi) * cosf(theta);
    float z = radius * sinf(phi) * sinf(theta);
    float y = radius * cosf(phi);

    // Build the model matrix.
    ti::math::XMMATRIX world = ti::math::XMMatrixIdentity();

    // Build the view matrix.
    ti::math::XMVECTOR position = ti::math::XMVectorSet(x, y, z, 1.0f);
    ti::math::XMVECTOR target = ti::math::XMVectorZero();
    ti::math::XMVECTOR up = ti::math::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    ti::math::XMMATRIX view = ti::math::XMMatrixLookAtLH(position, target, up);

    // Build the projection matrix.
    ti::math::XMMATRIX projection = ti::math::XMMatrixPerspectiveFovLH(
        0.25f * ti::math::XM_PI, aspectRatio, 0.1f, 100.0f);

    // Build the MVP matrix.
    ti::math::XMMATRIX mvp = world * view * projection;

    // Update the buffer with the latest MVP matrix.
    ti::math::XMStoreFloat4x4(&objectMVP.mvp, XMMatrixTranspose(mvp));

    updateObjectMVP = true;
}
