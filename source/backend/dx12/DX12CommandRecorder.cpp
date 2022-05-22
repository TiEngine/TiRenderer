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

void DX12CommandRecorder::BeginRecord(PipelineState* const pipelineState)
{
    if (pipelineState) {
        DX12PipelineState* dxPSO = down_cast<DX12PipelineState*>(pipelineState);
        LogIfFailedF(recorder->Reset(allocator.Get(), dxPSO->PSO().Get()));
    } else {
        LogIfFailedF(recorder->Reset(allocator.Get(), NULL));
    }
}

void DX12CommandRecorder::EndRecord()
{
    LogIfFailedF(recorder->Close());
}

void DX12CommandRecorder::RcBarrier(
    InputVertex* const resource, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12InputVertex>(*this, *resource, before, after);
}

void DX12CommandRecorder::RcBarrier(
    InputIndex* const resource, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12InputIndex>(*this, *resource, before, after);
}

void DX12CommandRecorder::RcBarrier(
    ResourceBuffer* const resource, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12ResourceBuffer>(*this, *resource, before, after);
}

void DX12CommandRecorder::RcBarrier(
    ResourceImage* const resource, ResourceState before, ResourceState after)
{
    RcBarrierTemplate<DX12ResourceImage>(*this, *resource, before, after);
}

void DX12CommandRecorder::RcBarrier(
    Swapchain* const swapchain, ResourceState before, ResourceState after)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcBarrier:Swapchain);
    auto dxSwapchain = down_cast<DX12Swapchain*>(swapchain);
    recorder->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        // It is enough to use barrier for the RenderTargetBuffer alone,
        // and there is no need for DepthStencilBuffer, because that the
        // DepthStencilBuffer in Swaphain will not be used elsewhere.
        dxSwapchain->CurrentRenderTargetBuffer().Get(),
        ConvertResourceState(before), ConvertResourceState(after)));
}

void DX12CommandRecorder::RcUpload(const void* const data, size_t size,
    InputVertex* const destination, InputVertex* const staging)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:InputVertex);
    RcUploadTemplate<DX12InputVertex>(*this, *destination, *staging, size, data);
}

void DX12CommandRecorder::RcUpload(const void* const data, size_t size,
    InputIndex* const destination, InputIndex* const staging)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:InputIndex);
    RcUploadTemplate<DX12InputIndex>(*this, *destination, *staging, size, data);
}

void DX12CommandRecorder::RcUpload(const void* const data, size_t size,
    ResourceBuffer* const destination, ResourceBuffer* const staging)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:ResourceBuffer);
    RcUploadTemplate<DX12ResourceBuffer>(*this, *destination, *staging, size, data);
}

void DX12CommandRecorder::RcUpload(const void* const data, size_t size,
    ResourceImage* const destination, ResourceImage* const staging)
{
    CHECK_RECORD(description.type, CommandType::Transfer, RcUpload:ResourceImage);
    RcUploadTemplate<DX12ResourceImage>(*this, *destination, *staging, size, data);
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

void DX12CommandRecorder::RcClearColorAttachment(Swapchain* const swapchain)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearOutputColor:Swapchain);
    auto dxSwapchain = down_cast<DX12Swapchain*>(swapchain);
    recorder->ClearRenderTargetView(
        dxSwapchain->CurrentRenderTargetView(),
        dxSwapchain->RenderTargetClearValue().Color,
        0, NULL);
}

void DX12CommandRecorder::RcClearColorAttachment(ResourceImage* const attachment)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearOutputColor:ResourceImage);
}

void DX12CommandRecorder::RcClearDepthStencilAttachment(Swapchain* const swapchain)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearDepthStencil:Swapchain);
    auto dxSwapchain = down_cast<DX12Swapchain*>(swapchain);
    recorder->ClearDepthStencilView(
        dxSwapchain->CurrentDepthStencilView(),
        dxSwapchain->DepthStencilClearFlags(),
        dxSwapchain->DepthStencilClearValue().DepthStencil.Depth,
        dxSwapchain->DepthStencilClearValue().DepthStencil.Stencil,
        0, NULL);
}

void DX12CommandRecorder::RcClearDepthStencilAttachment(ResourceImage* const attachment)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcClearDepthStencil:ResourceImage);
}

void DX12CommandRecorder::RcSetRenderAttachments(
    Swapchain* const swapchain,
    const std::vector<Descriptor*>& colorAttachments,
    const std::vector<Descriptor*>& depthStencilAttachments,
    bool descriptorsContinuous)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetRenderAttachments);
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetDescriptors;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> depthStencilDescriptors;
    if (swapchain) {
        auto dxSwapchain = down_cast<DX12Swapchain*>(swapchain);
        renderTargetDescriptors.emplace_back(dxSwapchain->CurrentRenderTargetView());
        if (dxSwapchain->IsSwapchainEnableDepthStencil()) {
            depthStencilDescriptors.emplace_back(dxSwapchain->CurrentDepthStencilView());
        }
    }
    for (auto attachment : colorAttachments) {
        auto dxDescriptor = down_cast<DX12Descriptor*>(attachment);
        renderTargetDescriptors.emplace_back(dxDescriptor->AttachmentView());
    }
    for (auto attachment : depthStencilAttachments) {
        auto dxDescriptor = down_cast<DX12Descriptor*>(attachment);
        depthStencilDescriptors.emplace_back(dxDescriptor->AttachmentView());
    }
    if (swapchain && renderTargetDescriptors.size() > 1) {
        descriptorsContinuous = false;
    }
    recorder->OMSetRenderTargets(static_cast<UINT>(renderTargetDescriptors.size()),
        renderTargetDescriptors.data(), descriptorsContinuous,
        depthStencilDescriptors.data());
}

void DX12CommandRecorder::RcSetVertex(
    const std::vector<InputVertex*>& vertices,
    InputVertexAttributes* const attributes, unsigned int startSlot)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetVertex);
    std::vector<DX12InputVertex*> dxVertices(vertices.size());
    for (size_t n = 0; n < vertices.size(); n++) {
        dxVertices[n] = down_cast<DX12InputVertex*>(vertices[n]);
    }
    auto dxAttributes = down_cast<DX12InputVertexAttributes*>(attributes);
    std::vector<D3D12_VERTEX_BUFFER_VIEW> bufferViews(dxVertices.size());
    for (size_t n = 0; n < dxVertices.size(); n++) {
        bufferViews[n] = dxVertices[n]->BufferView(dxAttributes);
    }
    recorder->IASetVertexBuffers(startSlot,
        static_cast<UINT>(dxVertices.size()), bufferViews.data());
}

void DX12CommandRecorder::RcSetIndex(
    InputIndex* const index, InputIndexAttribute* const attribute)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetIndex);
    auto dxIndex = down_cast<DX12InputIndex*>(index);
    auto dxAttribute = down_cast<DX12InputIndexAttribute*>(attribute);
    recorder->IASetIndexBuffer(&dxIndex->BufferView(dxAttribute));
    recorder->IASetPrimitiveTopology(dxAttribute->GetIndexInformation().PrimitiveTopology);
}

void DX12CommandRecorder::RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetDescriptorHeap);

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
    recorder->SetDescriptorHeaps(static_cast<UINT>(
        descriptorHeaps.size()), descriptorHeaps.data());
}

void DX12CommandRecorder::RcSetDescriptor(unsigned int index, ResourceBuffer* const resource)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetDescriptor:ResourceBuffer);
    auto dxResource = down_cast<DX12ResourceBuffer*>(resource);
    recorder->SetGraphicsRootConstantBufferView(index,
        dxResource->Buffer()->GetGPUVirtualAddress());
}

void DX12CommandRecorder::RcSetDescriptor(unsigned int index, ResourceImage* const resource)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetDescriptor:ResourceImage);
    // TODO
}

void DX12CommandRecorder::RcSetDescriptors(unsigned int index,
    const std::vector<Descriptor*>& descriptors)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetDescriptor:Descriptors);
    if (descriptors.size() <= 0) {
        TI_LOG_RET_W(TAG, "Records RcSetDescriptors is not executed, input descriptors is empty.");
    }
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dxDescriptorsHandles;
    for (auto descriptor : descriptors) {
        auto dxDescriptor = down_cast<DX12Descriptor*>(descriptor);
        dxDescriptorsHandles.emplace_back(dxDescriptor->NativeCpuDescriptor());
    }
    auto dxBaseDescriptor = down_cast<DX12Descriptor*>(descriptors[0]);
    if (dxBaseDescriptor->IsNativeDescriptorsContinuous(dxDescriptorsHandles)) {
        recorder->SetGraphicsRootDescriptorTable(index, dxBaseDescriptor->NativeGpuDescriptor());
    } else {
        TI_LOG_RET_E(TAG, "Records RcSetDescriptors failed, descriptors is not continuous.");
    }
}

void DX12CommandRecorder::RcSetPipelineLayout(PipelineLayout* const layout)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcSetPipelineLayout);
    recorder->SetGraphicsRootSignature(down_cast<DX12PipelineLayout*>(layout)->Signature().Get());
}

void DX12CommandRecorder::RcDraw(InputIndex* const index)
{
    CHECK_RECORD(description.type, CommandType::Graphics, RcDraw);
    recorder->DrawIndexedInstanced(down_cast<DX12InputIndex*>(index)->IndicesCount(), 1, 0, 0, 0);
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
