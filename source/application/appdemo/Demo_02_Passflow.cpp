#include "DemosBaseHeader.h"

namespace {

const std::string VertexShaderString = R"(
cbuffer cbPerObject : register(b0)
{
    float4x4 gModelViewProj;
};

StructuredBuffer<float4> inColor : register(t0);

struct VertexIn
{
    float3 inLocalPos : POSITION;
    uint   vertexId   : SV_VertexID;
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
    vout.outColor = inColor[vertexId];
    // Output to next pass.
    return vout;
}
)";

const std::string FragmentShaderString = R"(
Texture2D gHalfTexture : register(t1, space3);
SamplerState gSimpleSampler : register(s2, space3);

struct VertexOut
{
    float4 outWorldPos : SV_POSITION;
    float4 outColor    : COLOR;
};

struct PixelOut
{
    float4 color     : SV_Target0;
    float4 halfColor : SV_Target1;
};

PixelOut Main(VertexOut pin)
{
    PixelOut pout;
    pout.color = pin.outColor;
    pout.halfColor = pin.outColor * gHalfTexture.Sample(gSimpleSampler, float2(0.0f, 0.0f));
    return pout;
}
)";

}

class DrawPass : public ti::passflow::RasterizePass {
public:
    DrawPass(ti::passflow::Passflow& passflow);
    ~DrawPass() override;

protected:
    void OnPreparePass(ti::backend::Device* device) override;
    void OnBeforePass(unsigned int currentBufferingIndex) override;
    void OnExecutePass(ti::backend::CommandRecorder* recorder) override;
    void OnAfterPass(unsigned int currentPassInFlowIndex) override;
    void OnEnablePass(bool enable) override;

private:
    std::unique_ptr<InputProperties> inputProperties;
    std::unique_ptr<OutputProperties> outputProperties;
    std::unique_ptr<ProgramProperties> programProperties;
    std::unique_ptr<ShaderResourceProperties> resourceProperties;
};

DrawPass::DrawPass(ti::passflow::Passflow& passflow) : RasterizePass(passflow)
{
}

DrawPass::~DrawPass()
{
}

void DrawPass::OnPreparePass(ti::backend::Device* device)
{
    InitializePipeline(device);

    inputProperties = std::make_unique<InputProperties>(InputProperties{
        { InputProperties::MakeDefaultPositionVertexAttribute() },
        { ti::IndexFormat::UINT16,
          ti::PrimitiveTopology::TRIANGLE_LIST,
          ti::IndexStripCutValue::UINT16_MAX_VALUE },
        true // Support to add multiple draw items, default is true.
    });
    DeclareInput(*inputProperties);

    outputProperties = std::make_unique<OutputProperties>();
    outputProperties->targets[OutputProperties::OutputSlot::DS] = {
        ti::BasicFormat::D24_UNORM_S8_UINT,
        ti::PassAction::Clear,
        ti::PassAction::Store
    };
    outputProperties->targets[OutputProperties::OutputSlot::C0] = {
        ti::BasicFormat::R32G32B32A32_FLOAT,
        ti::PassAction::Clear,
        ti::PassAction::Store
    };
    outputProperties->targets[OutputProperties::OutputSlot::C1] = {
        ti::BasicFormat::R32G32B32A32_FLOAT,
        ti::PassAction::Clear,
        ti::PassAction::Store
    };
    DeclareOutput(*outputProperties);

    programProperties = std::make_unique<ProgramProperties>();
    programProperties->shaders[ti::ShaderStage::Vertex] = VertexShaderString;
    programProperties->shaders[ti::ShaderStage::Pixel] = FragmentShaderString;
    DeclareProgram(*programProperties);

    resourceProperties = std::make_unique<ShaderResourceProperties>();
    resourceProperties->resources[ShaderResourceProperties::ResourceSpace::PerObject] = {
        { // The object vertex color
            0,                                   // baseBindingPoint
            1,                                   // bindingPointCount
            ti::ShaderStage::Graphics,           // resourceVisibility
            ti::DescriptorType::StorageBuffer,   // resourceType
            ti::ResourceState::GENERAL_READ,     // beforeState
            ti::ResourceState::GENERAL_READ      // afterState
        },
        { // The object MVP matrix
            1,                                   // baseBindingPoint
            1,                                   // bindingPointCount
            ti::ShaderStage::Graphics,           // resourceVisibility
            ti::DescriptorType::ConstantBuffer,  // resourceType
            ti::ResourceState::GENERAL_READ,     // beforeState
            ti::ResourceState::GENERAL_READ      // afterState
        }
    };
    resourceProperties->resources[ShaderResourceProperties::ResourceSpace::PerPass] = {
        { // A test texture
            0,                                   // baseBindingPoint
            1,                                   // bindingPointCount
            ti::ShaderStage::Graphics,           // resourceVisibility
            ti::DescriptorType::ReadOnlyTexture, // resourceType
            ti::ResourceState::GENERAL_READ,     // beforeState
            ti::ResourceState::GENERAL_READ      // afterState
        },
        { // A texture sampler
            1,                                   // baseBindingPoint
            1,                                   // bindingPointCount
            ti::ShaderStage::Graphics,           // resourceVisibility
            ti::DescriptorType::ImageSampler,    // resourceType
            ti::ResourceState::GENERAL_READ,     // beforeState
            ti::ResourceState::GENERAL_READ      // afterState
        }
    };
    DeclareResource(*resourceProperties);

    BuildPipeline();
}

void DrawPass::OnBeforePass(unsigned int currentBufferingIndex)
{
    UpdateDrawItems(currentBufferingIndex);
    UpdateFrameResources(currentBufferingIndex);
}

void DrawPass::OnExecutePass(ti::backend::CommandRecorder* recorder)
{
}

void DrawPass::OnAfterPass(unsigned int currentPassInFlowIndex)
{
}

void DrawPass::OnEnablePass(bool enable)
{
}

void Demo_02_Passflow::Begin()
{
    passflow = std::make_unique<ti::passflow::Passflow>("[Demo][Passflow]");
    drawpass = passflow->CreateOrGetPass<DrawPass>("[Demo][DrawPass]");
    passflow->EnablePass(passflow->AddPassToFlow(drawpass), true);
}
