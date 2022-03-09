#include "DX12ResourceImage.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"

namespace ti::backend {
DX12ResourceImage::DX12ResourceImage(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceImage::~DX12ResourceImage()
{
    Shutdown();
}

void DX12ResourceImage::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 image: %p", this);
    this->description = description;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = description.width;
    resourceDesc.Height = description.height;
    resourceDesc.DepthOrArraySize = description.arrays;
    resourceDesc.MipLevels = description.mips;
    resourceDesc.Format = ConvertBasicFormat(description.format);
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = ConvertImageResourceFlag(description.type);
    D3D12_CLEAR_VALUE clearValue = ConvertClearValue(description.format, description.clearValue);

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &resourceDesc,
        ConvertResourceState(ResourceState::GENERAL_READ),
        &clearValue, IID_PPV_ARGS(&buffer)));

    if (description.memoryType == TransferDirection::GPU_ONLY) {
        LogIfFailedF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE, &resourceDesc,
            ConvertResourceState(ResourceState::GENERAL_READ),
            &clearValue, IID_PPV_ARGS(&uploader)));
        transfer = down_cast<DX12CommandRecorder*>(
            internal.CreateCommandRecorder({ CommandType::Transfer }));
    }
}

void DX12ResourceImage::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 image: %p", this);
    description = {
        BasicFormat::R32G32B32A32_FLOAT,
        ImageType::Color, 0u, 0u };
    buffer.Reset();
    uploader.Reset();
    downloader.Reset();
    if (transfer) {
        internal.DestroyCommandRecorder(transfer);
        transfer = nullptr;
    }
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceImage::Buffer()
{
    return buffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceImage::Uploader()
{
    return uploader;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceImage::Downloader()
{
    return downloader;
}

}
