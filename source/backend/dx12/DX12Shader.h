#pragma once

#include "backend/Shader.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Shader : public Shader
    , DX12Object<DX12Shader> {
public:
    explicit DX12Shader();
    ~DX12Shader() override;

    void Setup(Description description);
    void Shutdown();

    std::string DumpBytecode() const override;

    Reflection Reflect() const override;

    Microsoft::WRL::ComPtr<ID3DBlob> GetBytecode();

protected:
    void ProcessSource();
    void ProcessBytecode();

private:
    Description description{ ShaderStage::Vertex, "" };
    Microsoft::WRL::ComPtr<ID3DBlob> bytecode;
};
}
