#include "DX12CommandRecorder.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"
#include "DX12InputVertex.h"
#include "DX12InputIndex.h"
#include "DX12ResourceBuffer.h"

#define CHECK_RECORD(check, standard, information)                 \
do {                                                               \
    if (!(common::EnumCast(check) & common::EnumCast(standard))) { \
        TI_LOG_W(TAG, "CheckRecordFailed: <line:%d><info:%s> "     \
            "The current command type is not suitable for %s.",    \
                __LINE__, #information, #standard);                \
    }                                                              \
} while(0)

namespace {
using namespace ti::backend;

template <typename Implement, typename Interface>
inline void RcBarrierTemplate(DX12CommandRecorder& recorder,
    Interface& input, ResourceState before, ResourceState after)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "RcBarrierTemplate: Implement should inherit from Interface!");
    Implement& impl = down_cast<Implement&>(input);
    recorder.CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        impl.Buffer().Get(), ConvertResourceState(before), ConvertResourceState(after)));
}

template <typename Implement, typename Interface>
inline void RcUploadTemplate(DX12CommandRecorder& recorder,
    Interface& input, const std::vector<uint8_t>& pendingData)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "RcUploadTemplate: Implement should inherit from Interface!");
    Implement& impl = down_cast<Implement&>(input);
    D3D12_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pData = pendingData.data();
    subResourceData.RowPitch = pendingData.size();
    subResourceData.SlicePitch = subResourceData.RowPitch;
    UpdateSubresources<1>(recorder.CommandList().Get(),
        impl.Buffer().Get(), impl.Uploader().Get(), 0, 0, 1, &subResourceData);
}
}

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
    //   commandList.Close();
    //   ExecuteCommandLists...
    LogIfFailedF(recorder->Close());

    // This Fence is used to synchronize and wait until this
    // CommandRecord recorded commands is executed.
    LogIfFailedF(device->CreateFence(currentFence,
        D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
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

void DX12CommandRecorder::BeginRecord(const PipelineState* pipelineState)
{
    if (pipelineState) {
        //ID3D12PipelineState* pso = down_cast<DX12PipelineState*>(&pipelineState)->;
    } else {
        LogIfFailedF(recorder->Reset(allocator.Get(), NULL));
    }
}

void DX12CommandRecorder::EndRecord()
{
    LogIfFailedF(recorder->Close());
}

void DX12CommandRecorder::RcBarrier(InputVertex& input, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12InputVertex>(*this, input, before, after);
}

void DX12CommandRecorder::RcBarrier(InputIndex& input, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12InputIndex>(*this, input, before, after);
}

void DX12CommandRecorder::RcBarrier(ResourceBuffer& buffer, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12ResourceBuffer>(*this, buffer, before, after);
}

void DX12CommandRecorder::RcUpload(InputVertex& input, const std::vector<uint8_t>& data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:InputVertex);
    RcUploadTemplate<DX12InputVertex>(*this, input, data);
}

void DX12CommandRecorder::RcUpload(InputIndex& input, const std::vector<uint8_t>& data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:InputIndex);
    RcUploadTemplate<DX12InputIndex>(*this, input, data);
}

void DX12CommandRecorder::RcUpload(ResourceBuffer& buffer, const std::vector<uint8_t>& data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:ResourceBuffer);
    RcUploadTemplate<DX12ResourceBuffer>(*this, buffer, data);
}

void DX12CommandRecorder::Submit()
{
    ID3D12CommandList* pCommandLists[] = { recorder.Get() };
    queue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);
    currentFence++; // This fence is held by CommandRecorder.
    LogIfFailedF(queue->Signal(fence.Get(), currentFence));
}

void DX12CommandRecorder::Wait()
{
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

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DX12CommandRecorder::CommandList()
{
    return recorder;
}
}
