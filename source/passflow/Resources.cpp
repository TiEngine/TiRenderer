#include "passflow/Resources.h"
#include "general/Backend.h"

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
    SafeCopyMemory(buffer->Map(), description.bufferBytesSize,
                   RawPtr(),      description.bufferBytesSize);
    buffer->Unmap();
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
    auto staging = device->CreateInputVertex({
        description.verticesCount,
        description.attributesByteSize,
        ti::TransferDirection::CPU_TO_GPU });
    auto command = device->CreateCommandRecorder({ "Upload", ti::CommandType::Transfer });

    size_t totalByteSize = static_cast<size_t>(description.verticesCount)
                         * static_cast<size_t>(description.attributesByteSize);

    command->BeginRecord();
    command->RcBarrier(vertex,
        ti::ResourceState::GENERAL_READ,
        ti::ResourceState::COPY_DESTINATION);
    command->RcUpload(RawPtr(), totalByteSize, vertex, staging);
    command->RcBarrier(vertex,
        ti::ResourceState::COPY_DESTINATION,
        ti::ResourceState::GENERAL_READ);
    command->EndRecord();
    command->Submit();
    command->Wait();

    device->ReleaseCommandRecordersMemory("Upload");
    device->DestroyCommandRecorder(command);
    device->DestroyInputVertex(staging);
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
