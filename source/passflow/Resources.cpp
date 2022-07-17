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

backend::ResourceBuffer* BaseConstantBuffer::Buffer()
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
// BaseConstantBuffer

}
