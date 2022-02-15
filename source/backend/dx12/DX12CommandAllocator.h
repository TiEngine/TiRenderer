#pragma once

#include "backend/CommandAllocator.h"
#include "DX12BackendHeaders.h"
#include "DX12CommandList.h"

namespace ti::backend {
class DX12CommandAllocator : public CommandAllocator {
public:
    explicit DX12CommandAllocator(
        Microsoft::WRL::ComPtr<ID3D12Device> device);
    ~DX12CommandAllocator() override;

    void Setup(Description description);
    void Shutdown();

    CommandList* CreateCommandList(CommandList::Description description) override;
    bool DestroyCommandList(CommandList* commandList) override;

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;

    Description description;
    std::vector<std::unique_ptr<DX12CommandList>> commandLists;
};
}
