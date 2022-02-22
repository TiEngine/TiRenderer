#pragma once

#include "backend/Shader.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
class DX12Shader : public Shader {
public:
    explicit DX12Shader();
    ~DX12Shader() override;

    void Setup(Description description);
    void Shutdown();

    std::string DumpBytecode() const override;

    Microsoft::WRL::ComPtr<ID3DBlob> GetBytecode();

protected:
    void ProcessSource();
    void ProcessBytecode();

private:
    Description description{ ShaderStage::Vertex, "" };
    Microsoft::WRL::ComPtr<ID3DBlob> bytecode;
};
}
