#include "DX12PipelineState.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace ti::backend {
DX12PipelineState::DX12PipelineState(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12PipelineState::~DX12PipelineState()
{
    Shutdown();
}

void DX12PipelineState::Setup(Description description)
{
    ZeroMemory(&pipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    //--------------------- D3D12_GRAPHICS_PIPELINE_STATE_DESC ---------------------
    // ID3D12RootSignature* pRootSignature;                 - SetPipelineLayout
    // D3D12_SHADER_BYTECODE VS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE PS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE DS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE HS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE GS;                            - NULL and 0 / SetShader
    // D3D12_STREAM_OUTPUT_DESC StreamOutput;               - NULL and 0
    // D3D12_BLEND_DESC BlendState;                         - Default / SetBlendState..
    // UINT SampleMask;                                     - Default: UINT_MAX
    // D3D12_RASTERIZER_DESC RasterizerState;               - Default / SetRasterizerState..
    // D3D12_DEPTH_STENCIL_DESC DepthStencilState;          - Default / SetDepthStencilState..
    // D3D12_INPUT_LAYOUT_DESC InputLayout;                 - Default: NULL & 0 / SetVertexAssembly
    // D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;  - Default: Disable  / SetIndexAssembly
    // D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType; - Default: Triangle / SetIndexAssembly
    // UINT NumRenderTargets;                               - Initial: 0 / BuildState
    // DXGI_FORMAT RTVFormats[8];                     - DXGI_FORMAT_UNKNOWN(0) / SetColorAttachment
    // DXGI_FORMAT DSVFormat;                  - DXGI_FORMAT_UNKNOWN(0) / SetDepthStencilAttachment
    // DXGI_SAMPLE_DESC SampleDesc;            - Default: MSAAx1(1) & 0 / SetMSAA
    // UINT NodeMask;                          - 0
    // D3D12_CACHED_PIPELINE_STATE CachedPSO;  - NULL & 0 / PipelineState::Description::cache
    // D3D12_PIPELINE_STATE_FLAGS Flags;       - 0
    //------------------------------------------------------------------------------
    pipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipelineState.SampleMask = UINT_MAX;
    pipelineState.SampleDesc.Count = 1;
    pipelineState.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

void DX12PipelineState::Shutdown()
{
    pipelineStateObject.Reset();
}

void DX12PipelineState::SetPipelineLayout(PipelineLayout* layout)
{
    auto dxLayout = down_cast<DX12PipelineLayout*>(layout);
    pipelineState.pRootSignature = dxLayout->Signature().Get();
}

void DX12PipelineState::SetIndexAssembly(InputIndexAttribute* iia)
{
    auto dxIia = down_cast<DX12InputIndexAttribute*>(iia);
    pipelineState.IBStripCutValue = dxIia->GetIndexInformation().StripCutValue;
    pipelineState.PrimitiveTopologyType = dxIia->GetIndexInformation().PrimitiveTopologyType;
}

void DX12PipelineState::SetVertexAssembly(InputVertexAttributes* iva)
{
    auto dxIva = down_cast<DX12InputVertexAttributes*>(iva);
    pipelineState.InputLayout = {
        dxIva->GetInputElements().data(),
        static_cast<UINT>(dxIva->GetInputElements().size())
    };
}

void DX12PipelineState::SetShader(ShaderStage stage, Shader* shader)
{
    auto dxShader = down_cast<DX12Shader*>(shader);
    auto bytecodeBlob = dxShader->GetBytecode();
    D3D12_SHADER_BYTECODE bytecode{
        bytecodeBlob->GetBufferPointer(),
        bytecodeBlob->GetBufferSize()
    };
    switch (stage) {
    case ShaderStage::Vertex:   pipelineState.VS = bytecode; break;
    case ShaderStage::Hull:     pipelineState.HS = bytecode; break;
    case ShaderStage::Domain:   pipelineState.DS = bytecode; break;
    case ShaderStage::Geometry: pipelineState.GS = bytecode; break;
    case ShaderStage::Pixel:    pipelineState.PS = bytecode; break;
    default: TI_LOG_RET_W(TAG, "Pipeline state set shader failed, invalid stage!");
    }
}

void DX12PipelineState::SetColorAttachment(unsigned int location, BasicFormat format)
{
    if (location >= (sizeof(pipelineState.RTVFormats) / sizeof(pipelineState.RTVFormats[0]))) {
        TI_LOG_RET_W(TAG, "Pipeline state set color attachment failed, location overflow!");
    }
    pipelineState.RTVFormats[location] = ConvertBasicFormat(format);
    // Scan all render targets format to detect and update render targets count.
    pipelineState.NumRenderTargets = 0;
    for (DXGI_FORMAT renderTargetFormat : pipelineState.RTVFormats) {
        if (renderTargetFormat != DXGI_FORMAT_UNKNOWN) {
            pipelineState.NumRenderTargets++;
        }
    }
}

void DX12PipelineState::SetDepthStencilAttachment(BasicFormat format)
{
    pipelineState.DSVFormat = ConvertBasicFormat(format);
}

void DX12PipelineState::SetRasterizerState(RasterizerState state)
{
    pipelineState.RasterizerState = ConvertRasterizerState(state);
}

void DX12PipelineState::SetRasterizerStateFillMode(FillMode mode)
{
    pipelineState.RasterizerState.FillMode = ConvertFillMode(mode);
}

void DX12PipelineState::SetRasterizerStateCullMode(CullMode mode)
{
    pipelineState.RasterizerState.CullMode = ConvertCullMode(mode);
}

void DX12PipelineState::SetMSAA(MSAA msaa)
{
    pipelineState.SampleDesc.Count = ConvertMSAA(msaa);
}

void DX12PipelineState::BuildState()
{
    LogIfFailedE(device->CreateGraphicsPipelineState(
        &pipelineState, IID_PPV_ARGS(&pipelineStateObject)));
}


}
