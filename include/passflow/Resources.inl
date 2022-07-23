#include "Resources.h"

namespace ti::passflow {

inline void SafeCopyMemory(void* dstAddr, size_t dstSize, const void* srcAddr, size_t srcSize)
{
    #ifdef _MSC_VER
    memcpy_s(dstAddr, dstSize, srcAddr, srcSize);
    #else
    memcpy(dstAddr, srcAddr, std::min(dstSize, srcSize));
    #endif
}

//////////////////////////////////////////////////
// ConstantBuffer<T>

template <typename T>
inline void ConstantBuffer<T>::SetupConstantBuffer()
{
    constantBufferValue = std::make_unique<T>();
    description.bufferBytesSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline T& ConstantBuffer<T>::AcquireConstantBuffer()
{
    return *constantBufferValue;
}

template <typename T>
inline void ConstantBuffer<T>::UpdateConstantBuffer(const T& value)
{
    SafeCopyMemory(constantBufferValue.get(), sizeof(T), &value, sizeof(value));
}

template <typename T>
inline void* ConstantBuffer<T>::RawPtr()
{
    return constantBufferValue.get();
}

//////////////////////////////////////////////////
// IndexBuffer

template <typename T>
inline void IndexBuffer<T>::SetupIndexBuffer(unsigned int indicesCount)
{
    indexBufferValue.resize(indicesCount);
    description.indicesCount = indicesCount;
    description.indexByteSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline std::vector<T>& IndexBuffer<T>::AcquireIndexBuffer()
{
    return indexBufferValue;
}

template <typename T>
void IndexBuffer<T>::UpdateIndexBuffer(const std::vector<T>& value, unsigned int offset)
{
    SafeCopyMemory(
        indexBufferValue.data() + offset,
        (indexBufferValue.size() - offset) * sizeof(T),
        value.data(),
        value.size() * sizeof(T));
}

template <typename T>
inline void* IndexBuffer<T>::RawPtr()
{
    return indexBufferValue.data();
}

//////////////////////////////////////////////////
// VertexBuffer

template <typename T>
inline void VertexBuffer<T>::SetupVertexBuffer(unsigned int verticesCount)
{
    vertexBufferValue.resize(verticesCount);
    description.verticesCount = verticesCount;
    description.attributesByteSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline std::vector<T>& VertexBuffer<T>::AcquireVertexBuffer()
{
    return vertexBufferValue;
}

template <typename T>
inline void VertexBuffer<T>::UpdateVertexBuffer(const std::vector<T>& value, unsigned int offset)
{
    SafeCopyMemory(
        vertexBufferValue.data() + offset,
        (vertexBufferValue.size() - offset) * sizeof(T),
        value.data(),
        value.size() * sizeof(T));
}

template <typename T>
inline void* VertexBuffer<T>::RawPtr()
{
    return vertexBufferValue.data();
}

}
