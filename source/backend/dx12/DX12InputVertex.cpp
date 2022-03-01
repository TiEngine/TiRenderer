#include "DX12InputVertex.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"

namespace ti::backend {
DX12InputVertex::DX12InputVertex(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12InputVertex::~DX12InputVertex()
{
    Shutdown();
}

void DX12InputVertex::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 vertex: %p", this);
    this->description = description;

    unsigned int bytesSize = description.verticesCount * description.attributesByteSize;
    if (bytesSize == 0) {
        TI_LOG_RET_F(TAG, "Create vertex buffer failed, buffer size is zero!");
    }

    ResourceState initial= SelectInitialResourceState(description.memoryType);
    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bytesSize),
        ConvertResourceState(initial), NULL, IID_PPV_ARGS(&buffer)));
    SetState(initial);

    if (description.memoryType == TransferDirection::GPU_ONLY) {
        LogIfFailedF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bytesSize),
            D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploader)));
    }

    transfer = down_cast<DX12CommandRecorder*>(
        internal.CreateCommandRecorder({ CommandType::Transfer }));
}

void DX12InputVertex::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 vertex: %p", this);
    description = { 0u, 0u };
    buffer.Reset();
    uploader.Reset();
    downloader.Reset();
    internal.DestroyCommandRecorder(transfer);
    transfer = nullptr;
}

void DX12InputVertex::Upload(const std::vector<uint8_t>& data, bool sync)
{
    switch (description.memoryType) {
    case TransferDirection::GPU_ONLY:
        UploadGpuOnly(data, sync);
        return;
    case TransferDirection::CPU_TO_GPU:
        UploadCpuToGpu(data, sync);
        return;
    }
    TI_LOG_RET_F(TAG, "Upload vertex buffer failed, memory type mismatch.");
}

void DX12InputVertex::Readback(std::vector<uint8_t>& data)
{
    // TODO
}

void DX12InputVertex::SetState(ResourceState state)
{
    InputVertex::SetState(state);
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12InputVertex::Buffer()
{
    return buffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12InputVertex::Uploader()
{
    return uploader;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12InputVertex::Downloader()
{
    return downloader;
}

ResourceState DX12InputVertex::SelectInitialResourceState(TransferDirection memoryType)
{
    switch (memoryType) {
    case TransferDirection::GPU_ONLY:
        return ResourceState::UNDEFINED;
    case ti::backend::TransferDirection::CPU_TO_GPU:
        return ResourceState::GENERAL_READ;
    case ti::backend::TransferDirection::GPU_TO_CPU:
        return ResourceState::UNDEFINED; // FIXME
    }
    return ResourceState::UNDEFINED;
}

void DX12InputVertex::UploadGpuOnly(const std::vector<uint8_t>& data, bool forceSync)
{
    transfer->BeginRecord();
    transfer->RcUpload(*this, data);
    transfer->EndRecord();
    transfer->Submit();
    if (forceSync) {
        transfer->Wait();
    }
}

void DX12InputVertex::UploadCpuToGpu(const std::vector<uint8_t>& data, bool forceSync)
{
    if (forceSync) {
        internal.WaitIdle();
    }
    uint8_t* mappedData = nullptr;
    LogIfFailedF(buffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
    CopyMemory(mappedData, data.data(), data.size());
    buffer->Unmap(0, nullptr);
}



}
