#pragma once

#include "backend/InputVertexAttributes.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
class DX12InputVertexAttributes : public InputVertexAttributes {
public:
    explicit DX12InputVertexAttributes();
    ~DX12InputVertexAttributes() override;

    void Setup(Description description);
    void Shutdown();

    void AddAttribute(Attribute attribute) override;

    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const;

protected:
    D3D12_INPUT_CLASSIFICATION SelectSlotClass(Attribute::SlotClass type) const;

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
    std::vector<std::string> semanticNames;
};
}
