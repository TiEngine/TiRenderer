#pragma once

#include "backend/Device.h"
#include "DX12Shader.h"
#include "DX12Swapchain.h"
#include "DX12CommandRecorder.h"
#include "DX12InputVertexAttributes.h"

namespace ti::backend {
class DX12Device : public Device {
public:
    explicit DX12Device(
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi);
    ~DX12Device() override;

    void Setup(Description description);
    void Shutdown();

    Shader* CreateShader(Shader::Description description) override;
    bool DestroyShader(Shader* shader) override;

    Swapchain* CreateSwapchain(Swapchain::Description description) override;
    bool DestroySwapchain(Swapchain* swapchain) override;

    CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) override;
    bool DestroyCommandRecorder(CommandRecorder* commandRecorder) override;

    InputVertexAttributes* CreateInputVertexAttributes(InputVertexAttributes::Description description) override;
    bool DestroyInputVertexAttributes(InputVertexAttributes* inputVertexAttributes) override;

    void WaitIdle() override;

    Microsoft::WRL::ComPtr<IDXGIFactory4> DXGIFactory();
    Microsoft::WRL::ComPtr<ID3D12Device> NativeDevice();
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue(CommandType type);
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator(CommandType type);
    void ResetCommandAllocator();

protected:
    // TODO:  Support select a adapter by custom.
    //        Currently only the default adapter is used.
    // FIXME: Adapters should not be placed in the DX12Device.
    //        Move it to DX12Context and add CreateAdapter function in DX12Context.
    void EnumAdapters();

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;

    Description description;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;

    UINT64 currentFence = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;

    std::vector<std::unique_ptr<DX12Shader>> shaders;
    std::vector<std::unique_ptr<DX12Swapchain>> swapchains;
    std::vector<std::unique_ptr<DX12CommandRecorder>> commandRecorders;
    std::vector<std::unique_ptr<DX12InputVertexAttributes>> inputVertexLayouts;
};
}
