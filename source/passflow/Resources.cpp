#include "passflow/Resources.h"
#include "general/Backend.h"

namespace {

LOG_TAG(passflow::Resources)

template <typename Resource>
void UploadHost(Resource* destination, const void* source, size_t size, size_t element = 1)
{
    auto totalBytes = size * element;
    ti::passflow::SafeCopyMemory(destination->Map(), totalBytes, source, totalBytes);
    destination->Unmap();
}

template <typename Resource>
void UploadRemote(ti::backend::Device* device,
    Resource* destination, Resource* staging, const void* source, size_t size, size_t element = 1)
{
    auto command = device->CreateCommandRecorder({ "Upload", ti::CommandType::Transfer });

    command->BeginRecord();
    command->RcBarrier(destination,
        ti::ResourceState::GENERAL_READ,
        ti::ResourceState::COPY_DESTINATION);

    auto totalBytes = size * element;
    command->RcUpload(source, totalBytes, destination, staging);

    command->RcBarrier(destination,
        ti::ResourceState::COPY_DESTINATION,
        ti::ResourceState::GENERAL_READ);
    command->EndRecord();

    command->Submit();
    command->Wait();

    device->ReleaseCommandRecordersMemory("Upload");
    device->DestroyCommandRecorder(command);
}

}

namespace ti::passflow {

DeviceHolder::DeviceHolder()
{
    device = Backend::GetReference().Device();
}

DeviceHolder::~DeviceHolder()
{
    device = nullptr;
}

//////////////////////////////////////////////////
// BaseConstantBuffer

BaseConstantBuffer::~BaseConstantBuffer()
{
    if (buffer) {
        device->DestroyResourceBuffer(buffer);
    }
}

void BaseConstantBuffer::UploadConstantBuffer()
{
    if (description.memoryType == TransferDirection::CPU_TO_GPU) {
        UploadHost(buffer, RawPtr(), description.bufferBytesSize);
    } else if (description.memoryType == TransferDirection::GPU_ONLY) {
        auto staging = device->CreateResourceBuffer({ description.bufferBytesSize });
        UploadRemote(device, buffer, staging, RawPtr(), description.bufferBytesSize);
        device->DestroyResourceBuffer(staging);
    } else {
        TI_LOG_W(TAG, "Upload constant buffer failed, this buffer is in the readback heap.");
    }
}

backend::ResourceBuffer* BaseConstantBuffer::RawInst()
{
    return buffer;
}

void BaseConstantBuffer::SetupGPU()
{
    if (!buffer) {
        buffer = device->CreateResourceBuffer(description);
    }
}

//////////////////////////////////////////////////
// BaseIndexBuffer

BaseIndexBuffer::~BaseIndexBuffer()
{
    if (index) {
        device->DestroyInputIndex(index);
    }
}

void BaseIndexBuffer::UploadIndexBuffer()
{
    if (description.memoryType == TransferDirection::GPU_ONLY) {
        auto staging = device->CreateInputIndex({ description.indicesCount,
            description.indexByteSize, ti::TransferDirection::CPU_TO_GPU });
        UploadRemote(device, index, staging, RawPtr(),
            description.indexByteSize, description.indicesCount);
        device->DestroyInputIndex(staging);
    } else if (description.memoryType == TransferDirection::CPU_TO_GPU) {
        UploadHost(index, RawPtr(), description.indexByteSize, description.indicesCount);
    } else {
        TI_LOG_W(TAG, "Upload index buffer failed, this buffer is in the readback heap.");
    }
}

backend::InputIndex* BaseIndexBuffer::RawInst()
{
    return index;
}

void BaseIndexBuffer::SetupGPU()
{
    if (!index) {
        index = device->CreateInputIndex(description);
    }
}

//////////////////////////////////////////////////
// BaseVertexBuffer

BaseVertexBuffer::~BaseVertexBuffer()
{
    if (vertex) {
        device->DestroyInputVertex(vertex);
    }
}

void BaseVertexBuffer::UploadVertexBuffer()
{
    if (description.memoryType == TransferDirection::GPU_ONLY) {
        auto staging = device->CreateInputVertex({ description.verticesCount,
            description.attributesByteSize, ti::TransferDirection::CPU_TO_GPU });
        UploadRemote(device, vertex, staging, RawPtr(),
            description.attributesByteSize, description.verticesCount);
        device->DestroyInputVertex(staging);
    } else if (description.memoryType == TransferDirection::CPU_TO_GPU) {
        UploadHost(vertex, RawPtr(), description.attributesByteSize, description.verticesCount);
    } else {
        TI_LOG_W(TAG, "Upload vertex buffer failed, this buffer is in the readback heap.");
    }
}

backend::InputVertex* BaseVertexBuffer::RawInst()
{
    return vertex;
}

void BaseVertexBuffer::SetupGPU()
{
    if (!vertex) {
        vertex = device->CreateInputVertex(description);
    }
}

}
