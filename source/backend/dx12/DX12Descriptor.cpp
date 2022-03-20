#include "DX12Descriptor.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"

namespace ti::backend {
    DX12Descriptor::DX12Descriptor(DX12Device& internal,
        DX12DescriptorHeap& heap, unsigned int index)
        : internal(internal), heap(heap)
{
    device = internal.NativeDevice();
}

    DX12Descriptor::~DX12Descriptor()
{
    Shutdown();
}

void DX12Descriptor::Setup(Description description)
{
    this->description = description;
}

void DX12Descriptor::Shutdown()
{
}

}
