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
        D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&buffer)));
    SetState(ResourceState::UNDEFINED);

    // In order to copy CPU memory data into GPU buffer,
    // we need to create an intermediate upload heap.
    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bytesSize),
        D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploader)));

    transfer = down_cast<DX12CommandRecorder*>(
        internal.CreateCommandRecorder({ CommandType::Transfer }));
}

void DX12InputVertex::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 vertex: %p", this);
    description = { 0u, 0u };
    buffer.Reset();
    uploader.Reset();
    internal.DestroyCommandRecorder(transfer);
    transfer = nullptr;
}

void DX12InputVertex::Upload(const std::vector<uint8_t>& data)
{
    switch (description.memoryType) {
    case TransferDirection::GPU_ONLY:
        UploadGpuOnly(data);
        return;
    case TransferDirection::CPU_TO_GPU:
        UploadCpuToGpu(data);
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

void DX12InputVertex::UploadGpuOnly(const std::vector<uint8_t>& data)
{
    // The upload buffer has to be kept alive after the following function calls
    // because the command list has not been executed yet that performs the actual copy.
    // Release upload buffer after it knows the copy has been executed(Submit and Wait).
    transfer->Reset();
    transfer->BeginRecord();
    transfer->RcUpload(*this, data);
    transfer->EndRecord();
    transfer->Submit();
    transfer->Wait();
}

void DX12InputVertex::UploadCpuToGpu(const std::vector<uint8_t>& data)
{
}

void DX12InputVertex::DownloadGpuOnly(std::vector<uint8_t>& data)
{
}

void DX12InputVertex::DownloadGpuToCpu(std::vector<uint8_t>& data)
{
}


}
