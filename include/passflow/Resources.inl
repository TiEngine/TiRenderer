#include "Resources.h"

namespace ti::passflow {

inline void SafeCopyMemory(
    void* dstAddr, unsigned int dstSize,
    void* srcAddr, unsigned int srcSize)
{
    #ifdef _MSC_VER
    memcpy_s(dstAddr, dstSize, srcAddr, srcSize);
    #else
    memcpy(dstAddr, srcAddr, std::min(dstSize, srcSize));
    #endif
}

//////////////////////////////////////////////////
// ConstantBuffer<T>

template<typename T>
inline void ConstantBuffer<T>::SetupConstantBuffer()
{
    constantBufferValue = std::make_unique<T>();
    description.bufferBytesSize = sizeof(T);
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

//////////////////////////////////////////////////
// MeshBuffer

inline void MeshBuffer::SetupMeshBuffer(unsigned int verticesCount)
{
    meshPositionBuffer.resize(verticesCount);
    description.verticesCount = verticesCount;
    description.attributesByteSize = sizeof(math::XMFLOAT3);
    SetupGPU();
}

inline std::vector<math::XMFLOAT3>& MeshBuffer::AcquireMeshBuffer()
{
    return meshPositionBuffer;
}

inline void MeshBuffer::UpdateMeshBuffer(
    const std::vector<math::XMFLOAT3>& value, unsigned int offset)
{
    //TODO
}

inline void* MeshBuffer::RawPtr()
{
    return meshPositionBuffer.data();
}

}
