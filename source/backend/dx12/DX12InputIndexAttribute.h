#pragma once

#include "backend/InputIndexAttribute.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12InputIndexAttribute : public InputIndexAttribute
    , DX12Object<DX12InputIndexAttribute> {
public:
    explicit DX12InputIndexAttribute();
    ~DX12InputIndexAttribute() override;

    void Setup(Description description);
    void Shutdown();

    void SetAttribute(Attribute attribute) override;

private:
    // TODO: Element...
};
}
