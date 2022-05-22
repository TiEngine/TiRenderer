#include <d3dcompiler.h>
#include "DX12Shader.h"
#include "DX12Common.h"

namespace ti::backend {
DX12Shader::DX12Shader()
{
}

DX12Shader::~DX12Shader()
{
    Shutdown();
}

void DX12Shader::Setup(Description description)
{
    this->description = description;

    switch (description.sourceType) {
    case Description::SourceType::Source:   ProcessSource();   break;
    case Description::SourceType::Bytecode: ProcessBytecode(); break;
    }
}

void DX12Shader::Shutdown()
{
    description = { ShaderStage::Vertex, "" };
    bytecode.Reset();
}

std::string DX12Shader::DumpBytecode() const
{
    if (bytecode != nullptr) {
        return std::string(static_cast<const char*>(
            bytecode->GetBufferPointer()), bytecode->GetBufferSize());
    } else {
        TI_LOG_W(TAG, "Dumping shader bytecode failed, shader not compiled!");
    }
    return std::string();
}

Microsoft::WRL::ComPtr<ID3DBlob> DX12Shader::GetBytecode()
{
    return bytecode;
}

void DX12Shader::ProcessSource()
{
    UINT compileFlags = 0;
    #if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #endif

    std::string target = "_5_1";
    switch (description.stage) {
    case ShaderStage::Vertex:   target = "vs" + target; break;
    case ShaderStage::Hull:     target = "hs" + target; break;
    case ShaderStage::Domain:   target = "ds" + target; break;
    case ShaderStage::Geometry: target = "gs" + target; break;
    case ShaderStage::Pixel:    target = "ps" + target; break;
    case ShaderStage::Compute:  target = "cs" + target; break;
    default: TI_LOG_RET_W(TAG, "Compile HLSL shader source failed! "
                               "No supported shader stage: %d", description.stage);
    }

    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    LogIfFailedW(D3DCompile(description.source.c_str(), description.source.size(),
        NULL, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "Main", target.c_str(),
        compileFlags, 0, &bytecode, &errors));

    if (errors != nullptr) {
        TI_LOG_W(TAG, "Compile HLSL shader source failed!\nsource:\n%s\nerror:\n%s",
            description.source.c_str(), errors->GetBufferPointer());
    }
}

void DX12Shader::ProcessBytecode()
{
    LogIfFailedW(D3DCreateBlob(description.source.size(), &bytecode));
    CopyMemory(bytecode->GetBufferPointer(), description.source.data(), description.source.size());
}
}
