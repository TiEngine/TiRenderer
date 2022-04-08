#include "DX12CommandRecorder.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

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
    Interface& resource, ResourceState before, ResourceState after)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "RcBarrierTemplate: Implement should inherit from Interface!");
    Implement& impl = down_cast<Implement&>(resource);
    recorder.CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        impl.Buffer().Get(), ConvertResourceState(before), ConvertResourceState(after)));
}

template <typename Implement, typename Interface>
inline void RcUploadTemplate(DX12CommandRecorder& recorder,
    Interface& destination, Interface& staging, size_t size, const void* data)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "RcUploadTemplate: Implement should inherit from Interface!");
    Implement& dest = down_cast<Implement&>(destination);
    Implement& stag = down_cast<Implement&>(staging);
    D3D12_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pData = data;
    subResourceData.RowPitch = size;
    subResourceData.SlicePitch = subResourceData.RowPitch;
    UpdateSubresources<1>(recorder.CommandList().Get(),
        dest.Buffer().Get(), stag.Buffer().Get(),
        0, 0, 1, &subResourceData);
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
    queue.Reset();
    allocator.Reset();
    recorder.Reset();
    currentFence = 0;
    fence.Reset();
}

void DX12CommandRecorder::BeginRecord(PipelineState* pipelineState)
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

void DX12CommandRecorder::RcBarrier(InputVertex& resource,
    ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12InputVertex>(*this, resource, before, after);
}

void DX12CommandRecorder::RcBarrier(InputIndex& resource,
    ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12InputIndex>(*this, resource, before, after);
}

void DX12CommandRecorder::RcBarrier(ResourceBuffer& resource,
    ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12ResourceBuffer>(*this, resource, before, after);
}

void DX12CommandRecorder::RcBarrier(ResourceImage& resource,
    ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12ResourceImage>(*this, resource, before, after);
}

void DX12CommandRecorder::RcUpload(InputVertex& destination,
    InputVertex& staging, size_t size, const void* data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:InputVertex);
    RcUploadTemplate<DX12InputVertex>(*this, destination, staging, size, data);
}

void DX12CommandRecorder::RcUpload(InputIndex& destination,
    InputIndex& staging, size_t size, const void* data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:InputIndex);
    RcUploadTemplate<DX12InputIndex>(*this, destination, staging, size, data);
}

void DX12CommandRecorder::RcUpload(ResourceBuffer& destination,
    ResourceBuffer& staging, size_t size, const void* data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:ResourceBuffer);
    RcUploadTemplate<DX12ResourceBuffer>(*this, destination, staging, size, data);
}

void DX12CommandRecorder::RcUpload(ResourceImage& destination,
    ResourceImage& staging, size_t size, const void* data)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:ResourceImage);
    RcUploadTemplate<DX12ResourceImage>(*this, destination, staging, size, data);
}

void DX12CommandRecorder::RcSetViewports(const std::vector<Viewport>& viewports)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetViewports);
    std::vector<D3D12_VIEWPORT> dxViewports;
    dxViewports.reserve(viewports.size());
    for (const auto& viewport : viewports) {
        dxViewports.emplace_back(D3D12_VIEWPORT{ viewport.x, viewport.y, // left top
            viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth });
    }
    recorder->RSSetViewports(static_cast<unsigned int>(dxViewports.size()), dxViewports.data());
}

void DX12CommandRecorder::RcSetScissors(const std::vector<Scissor>& scissors)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetScissors);
    std::vector<D3D12_RECT> dxScissors;
    dxScissors.reserve(scissors.size());
    for (const auto& scissor : scissors) {
        dxScissors.emplace_back(D3D12_RECT{
            scissor.left, scissor.top, scissor.right, scissor.bottom });
    }
    recorder->RSSetScissorRects(static_cast<unsigned int>(dxScissors.size()), dxScissors.data());
}

void DX12CommandRecorder::RcClearColorAttachment(Swapchain& swapchain)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearColorAttachment);
}

void DX12CommandRecorder::RcClearColorAttachment(ResourceImage& attachment)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearColorAttachment);
}

void DX12CommandRecorder::RcClearDepthStencilAttachment(Swapchain& swapchain)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearDepthStencilAttachment);
}

void DX12CommandRecorder::RcClearDepthStencilAttachment(ResourceImage& attachment)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearDepthStencilAttachment);
}

void DX12CommandRecorder::RcSetRenderAttachments(
    const std::vector<Swapchain*>& swapchains,
    const std::vector<ResourceImage*>& colorAttachments,
    const std::vector<ResourceImage*>& depthStencilAttachments,
    bool descriptorsContinuous)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetRenderAttachments);
}

void DX12CommandRecorder::RcSetVertex(const std::vector<InputVertex*>& vertices)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetVertex);
}

void DX12CommandRecorder::RcSetIndex(InputIndex* index)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetIndex);
}

void DX12CommandRecorder::RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps)
{
    std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
    descriptorHeaps.reserve(2);

    bool isCbvSrcUavHeapSetted = false;
    bool isSamplerHeapSetted = false;

    for (DescriptorHeap* heap : heaps) {
        auto dxHeap = down_cast<DX12DescriptorHeap*>(heap);
        switch (dxHeap->GetHeapType()) {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            if (!isCbvSrcUavHeapSetted) {
                descriptorHeaps.emplace_back(dxHeap->Heap().Get());
                isCbvSrcUavHeapSetted = true;
            }
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            if (!isSamplerHeapSetted) {
                descriptorHeaps.emplace_back(dxHeap->Heap().Get());
                isSamplerHeapSetted = true;
            }
            break;
        }
        if (isCbvSrcUavHeapSetted && isSamplerHeapSetted) {
            break;
        }
    }

    // The SetDescriptorHeaps interface of DX12CommandList
    // only allows one CBV_SRV_UAV heap and one SAMPLER heap!
    recorder->SetDescriptorHeaps(descriptorHeaps.size(), descriptorHeaps.data());
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
    // DX12Device::WaitIdle will wait until all commands on all command queues have been executed.
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
