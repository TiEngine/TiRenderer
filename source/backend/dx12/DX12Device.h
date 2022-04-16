#pragma once

#include "backend/Device.h"
#include "DX12Shader.h"
#include "DX12Swapchain.h"
#include "DX12CommandRecorder.h"
#include "DX12InputVertex.h"
#include "DX12InputVertexAttributes.h"
#include "DX12InputIndex.h"
#include "DX12InputIndexAttribute.h"
#include "DX12ResourceBuffer.h"
#include "DX12ResourceImage.h"
#include "DX12DescriptorHeap.h"
#include "DX12PipelineLayout.h"

namespace ti::backend {
class DX12Device : public Device
    , DX12Object<DX12Device> {
public:
    explicit DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi);
    ~DX12Device() override;

    void Setup(Description description);
    void Shutdown();

    Shader* CreateShader(Shader::Description description) override;
    bool DestroyShader(Shader* instance) override;

    Swapchain* CreateSwapchain(Swapchain::Description description) override;
    bool DestroySwapchain(Swapchain* instance) override;

    CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) override;
    bool DestroyCommandRecorder(CommandRecorder* instance) override;

    InputVertex* CreateInputVertex(InputVertex::Description description) override;
    bool DestroyInputVertex(InputVertex* instance) override;

    InputVertexAttributes* CreateInputVertexAttributes() override;
    bool DestroyInputVertexAttributes(InputVertexAttributes* instance) override;

    InputIndex* CreateInputIndex(InputIndex::Description description) override;
    bool DestroyInputIndex(InputIndex* instance) override;

    InputIndexAttribute* CreateInputIndexAttribute() override;
    bool DestroyInputIndexAttribute(InputIndexAttribute* instance) override;

    ResourceBuffer* CreateResourceBuffer(ResourceBuffer::Description description) override;
    bool DestroyResourceBuffer(ResourceBuffer* instance) override;

    //ResourceBufferEx* CreateResourceBuffer(ResourceBufferEx::Description description) override;
    //bool DestroyResourceBuffer(ResourceBufferEx* instance) override;

    ResourceImage* CreateResourceImage(ResourceImage::Description description) override;
    bool DestroyResourceImage(ResourceImage* instance) override;

    //ImageSampler* CreateImageSampler(ImageSampler::Description description) override;
    //bool DestroyImageSampler(ImageSampler* instance) override;

    DescriptorHeap* CreateDescriptorHeap(DescriptorHeap::Description description) override;
    bool DestroyDescriptorHeap(DescriptorHeap* instance) override;

    PipelineLayout CreatePipelineLayout(PipelineLayout::Description description) override;
    bool DestroyPipelineLayout(PipelineLayout* instance) override;

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
    std::vector<std::unique_ptr<DX12InputVertex>> inputVertices;
    std::vector<std::unique_ptr<DX12InputVertexAttributes>> inputVertexAttributes;
    std::vector<std::unique_ptr<DX12InputIndex>> inputIndices;
    std::vector<std::unique_ptr<DX12InputIndexAttribute>> inputIndexAttributes;
    std::vector<std::unique_ptr<DX12ResourceBuffer>> resourceBuffers;
    std::vector<std::unique_ptr<DX12ResourceImage>> resourceImages;
    std::vector<std::unique_ptr<DX12DescriptorHeap>> descriptorHeaps;
};
}
