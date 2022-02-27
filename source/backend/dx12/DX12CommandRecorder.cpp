#include "DX12CommandRecorder.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"
#include "DX12InputVertex.h"
#include "DX12InputIndex.h"

namespace ti::backend {
DX12CommandRecorder::DX12CommandRecorder(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12CommandRecorder::~DX12CommandRecorder()
{
    Shutdown();
}

void DX12CommandRecorder::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 command recorder: %p", this);
    this->description = description;

    queue = internal.CommandQueue(description.type);
    allocator = internal.CommandAllocator(description.type);

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
    queue.Reset();
    allocator.Reset();
    recorder.Reset();
    currentFence = 0;
    fence.Reset();
}

void DX12CommandRecorder::Reset(const PipelineState* pipelineState)
{
    if (pipelineState) {
    } else {
        recorder->Reset(allocator.Get(), NULL);
    }
    //ID3D12PipelineState* native = down_cast<DX12PipelineState*>(&pipelineState)->;
}

void DX12CommandRecorder::BeginRecord()
{
}

void DX12CommandRecorder::EndRecord()
{
}

void DX12CommandRecorder::RcBarrier(InputVertex& input, ResourceState before, ResourceState after)
{
    DX12InputVertex& vertex = down_cast<DX12InputVertex&>(input);
    if (vertex.GetState() != before) {
        TI_LOG_E(TAG, "Current resource state is not same with the before state!");
    }

    recorder->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertex.Buffer().Get(),
        ConvertResourceState(vertex.GetState()), ConvertResourceState(after)));
    vertex.SetState(after);
}

void DX12CommandRecorder::RcUpload(InputVertex& input, const std::vector<uint8_t>& data)
{
    if (description.type != CommandType::Transfer) {
        TI_LOG_W(TAG, "<RcUpload> <InputVertex> Current command type is not Transfer.");
    }

    D3D12_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pData = data.data();
    subResourceData.RowPitch = data.size();
    subResourceData.SlicePitch = subResourceData.RowPitch;

    DX12InputVertex& vertex = down_cast<DX12InputVertex&>(input);
    RcBarrier(vertex, vertex.GetState(), ResourceState::COPY_DESTINATION);
    UpdateSubresources<1>(recorder.Get(),
        vertex.Buffer().Get(), vertex.Uploader().Get(), 0, 0, 1, &subResourceData);
    RcBarrier(vertex, ResourceState::COPY_DESTINATION, ResourceState::GENERAL_READ);
}

void DX12CommandRecorder::RcUpload(InputIndex& input, const std::vector<uint8_t>& data)
{
    // TODO
}

void DX12CommandRecorder::Submit()
{
    LogIfFailedF(recorder->Close());
    ID3D12CommandList* pCommandLists[] = { recorder.Get() };
    queue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);
}

void DX12CommandRecorder::Wait(std::function<void()> coroutine)
{
    currentFence++; // This fence is held by CommandRecorder.
    LogIfFailedF(queue->Signal(fence.Get(), currentFence));

    if (coroutine) { // Process coroutine.
        coroutine();
    }

    // DX12Device::WaitIdle() will wait until all commands on all command queues have been executed.
    // The current Wait only waits until the command on the current command queue is executed.
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
