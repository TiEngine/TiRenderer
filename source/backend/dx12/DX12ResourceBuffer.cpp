#include "DX12ResourceBuffer.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"

namespace ti::backend {
DX12ResourceBuffer::DX12ResourceBuffer(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceBuffer::~DX12ResourceBuffer()
{
    Shutdown();
}

void DX12ResourceBuffer::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 constant buffer: %p", this);
    this->description = description;

    alignedBytesSize = CalculateConstantBufferBytesSize(description.bufferBytesSize);
    if (alignedBytesSize == 0) {
        TI_LOG_RET_F(TAG, "Create constant buffer failed, buffer size is zero!");
    }

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(alignedBytesSize),
        ConvertResourceState(ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));

    if (description.memoryType == TransferDirection::GPU_ONLY) {
        LogIfFailedF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(alignedBytesSize),
            ConvertResourceState(ResourceState::GENERAL_READ),
            NULL, IID_PPV_ARGS(&uploader)));
        transfer = down_cast<DX12CommandRecorder*>(
            internal.CreateCommandRecorder({ CommandType::Transfer }));
    }
}

void DX12ResourceBuffer::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 constant buffer: %p", this);
    description = { 0u };
    alignedBytesSize = 0;
    buffer.Reset();
    uploader.Reset();
    downloader.Reset();
    if (transfer) {
        internal.DestroyCommandRecorder(transfer);
        transfer = nullptr;
    }
}

void DX12ResourceBuffer::Upload(const std::vector<uint8_t>& data, bool sync)
{
    if (data.size() > alignedBytesSize) {
        TI_LOG_RET_F(TAG, "Upload constant buffer failed, size overflow."
            "Buffer size is %d, aligned size is %d, upload size is %d.",
            description.bufferBytesSize, alignedBytesSize, data.size());
    }
    switch (description.memoryType) {
    case TransferDirection::GPU_ONLY:
        UploadGpuOnly(data, sync);
        return;
    case TransferDirection::CPU_TO_GPU:
        UploadCpuToGpu(data, sync);
        return;
    }
    TI_LOG_RET_F(TAG, "Upload constant buffer failed, memory type mismatch.");
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceBuffer::Buffer()
{
    return buffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceBuffer::Uploader()
{
    return uploader;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceBuffer::Downloader()
{
    return downloader;
}

unsigned int DX12ResourceBuffer::CalculateConstantBufferBytesSize(unsigned int input)
{
    // Constant buffers must be a multiple of the minimum hardware allocation size,
    // it usually 256 bytes. So round up to nearest multiple of 256. We do this by
    // adding 255 and then masking off the lower two bytes which store all bits < 256.
    // Example: Suppose bytes size = 300, then:
    //   (300 + 255) & ~255
    //   555 & ~255
    //   0x022B & ~0x..000FF
    //   0x022B &  0x..FFF00
    //   0x0200
    //   512
    return (input + 255u) & ~255u;
}

void DX12ResourceBuffer::UploadGpuOnly(const std::vector<uint8_t>& data, bool forceSync)
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

void DX12ResourceBuffer::UploadCpuToGpu(const std::vector<uint8_t>& data, bool forceSync)
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