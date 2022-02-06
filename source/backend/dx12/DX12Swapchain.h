#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "backend/Swapchain.h"

namespace ti::backend {

class DX12Swapchain : public Swapchain {
public:
    void Setup(Description description) override;

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
};

}
