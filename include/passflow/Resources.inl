#include "Resources.h"

namespace ti::passflow {

void SafeCopyMemory(void* dstAddr, unsigned int dstSize, void* srcAddr, unsigned int srcSize)
{
    #ifdef _MSC_VER
    memcpy_s(dstAddr, dstSize, srcAddr, srcSize);
    #else
    memcpy(destination, source, std::min(dstSize, srcSize));
    #endif
}

//////////////////////////////////////////////////
// ConstantBuffer<T>

template<typename T>
inline void ConstantBuffer<T>::SetupConstantBuffer()
{
    description.bufferBytesSize = sizeof(value);
    SetupGPU();
}

template<typename T>
inline T& ConstantBuffer<T>::AcquireConstantBuffer()
{
    return constantBufferValue;
}

template<typename T>
inline void ConstantBuffer<T>::UpdateConstantBuffer(const T& value)
{
    constantBufferValue = value;
}

template<typename T>
inline void* ConstantBuffer<T>::RawPtr()
{
    return &constantBufferValue;
}

}
