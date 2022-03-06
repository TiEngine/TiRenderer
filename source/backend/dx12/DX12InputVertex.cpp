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

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bytesSize),
        ConvertResourceState(ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));

    if (description.memoryType == TransferDirection::GPU_ONLY) {
        LogIfFailedF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bytesSize),
            ConvertResourceState(ResourceState::GENERAL_READ),
            NULL, IID_PPV_ARGS(&uploader)));
        transfer = down_cast<DX12CommandRecorder*>(
            internal.CreateCommandRecorder({ CommandType::Transfer }));
    }
}

void DX12InputVertex::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 vertex: %p", this);
    description = { 0u, 0u };
    buffer.Reset();
    uploader.Reset();
    downloader.Reset();
    if (transfer) {
        internal.DestroyCommandRecorder(transfer);
        transfer = nullptr;
    }
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

void DX12InputVertex::UploadGpuOnly(const std::vector<uint8_t>& data, bool forceSync)
{
    transfer->BeginRecord();
    transfer->RcBarrier(*this, ResourceState::GENERAL_READ, ResourceState::COPY_DESTINATION);
    transfer->RcUpload(*this, data);
    transfer->RcBarrier(*this, ResourceState::COPY_DESTINATION, ResourceState::GENERAL_READ);
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
