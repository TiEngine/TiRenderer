#include "DX12CommandRecorder.h"
#include "DX12Common.h"
#include "common/TypeCast.hpp"

namespace ti::backend {
DX12CommandRecorder::DX12CommandRecorder(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue,
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
    : device(device), queue(queue), allocator(allocator)
{
}

DX12CommandRecorder::~DX12CommandRecorder()
{
    Shutdown();
}

void DX12CommandRecorder::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 command recorder: %p", this);
    this->description = description;

    LogIfFailedF(device->CreateCommandList(0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        allocator.Get(), // Associated command allocator
        NULL,            // Initial PipelineStateObject
        IID_PPV_ARGS(&recorder)));

    // Start with closed state.
    // The first time we refer to the command list will Reset
    // it, and it needs to be closed before calling Reset.
    // The process of recording command lists is:
    //   commandList.Reset(pipelineState);
    //   commandList.Xxx... // Commands
    //   commandList.Flush();
    recorder->Close();
}

void DX12CommandRecorder::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 command recorder: %p", this);
    recorder.Reset();
}

void DX12CommandRecorder::Reset(const PipelineState* pipelineState)
{
    if (pipelineState) {
    } else {
        recorder->Reset(allocator.Get(), NULL);
    }
    //ID3D12PipelineState* native = down_cast<DX12PipelineState*>(&pipelineState)->;
}

void DX12CommandRecorder::Submit()
{
    LogIfFailedF(recorder->Close());
    ID3D12CommandList* pCommandLists[] = { recorder.Get() };
    queue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);
}

void DX12CommandRecorder::Wait(std::function<void()> coroutine)
{
    currentFence++;
    LogIfFailedF(queue->Signal(fence.Get(), currentFence));

    if (coroutine) {
        coroutine();
    }

    if (fence->GetCompletedValue() < currentFence) {
        HANDLE eventHandle = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
        if (eventHandle != NULL) {
            LogIfFailedF(fence->SetEventOnCompletion(currentFence, eventHandle));
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        } else {
            TI_LOG_F(TAG, "Command recorder wait failed, can not create event!");
        }
    }
}
}
