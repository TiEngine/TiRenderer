#pragma once

#include <memory>
#include <unordered_map>
#include "math/Math.hpp"
#include "common/Logger.hpp"
#include "backend/BackendContext.h"

namespace ti::passflow {

class DeviceHolder {
public:
    DeviceHolder();
    virtual ~DeviceHolder() = 0;

protected:
    backend::Device* device; // Not owned!
};

class BaseConstantBuffer : public DeviceHolder {
public:
    BaseConstantBuffer() = default;
    ~BaseConstantBuffer() override;

    void UploadConstantBuffer();

    virtual void* RawPtr() = 0;
    backend::ResourceBuffer* RawInst();

protected:
    void SetupGPU();

    backend::ResourceBuffer::Description description{ 0u }; // Memory type: CPU_TO_GPU
    backend::ResourceBuffer* buffer = nullptr;
};

class BaseStructuredBuffer {
    // TODO
};

class BaseReadWriteBuffer {
};

class BaseTexture {
};

class Texture2D final : public BaseTexture {
};

class ReadWriteTexture2D final : public BaseTexture {
};

class ColorOutput final : public BaseTexture {
};

class DepthStencilOutput final : public BaseTexture {
};

class DisplayPresentOutput final {
};

////

class BaseIndexBuffer : public DeviceHolder {
public:
    BaseIndexBuffer() = default;
    ~BaseIndexBuffer() override;

    void UploadIndexBuffer();

    virtual void* RawPtr() = 0;
    backend::InputIndex* RawInst();

protected:
    void SetupGPU();

    backend::InputIndex::Description description{ 0u, 0u }; // Memory type: GPU_ONLY
    backend::InputIndex* index = nullptr;
};

class BaseVertexBuffer : public DeviceHolder {
public:
    BaseVertexBuffer() = default;
    ~BaseVertexBuffer() override;

    void UploadVertexBuffer();

    virtual void* RawPtr() = 0;
    backend::InputVertex* RawInst();

protected:
    void SetupGPU();

    backend::InputVertex::Description description{ 0u, 0u }; // Memory type: GPU_ONLY
    backend::InputVertex* vertex = nullptr;
};

template <typename T>
class ConstantBuffer final : public BaseConstantBuffer {
public:
    static_assert(std::is_trivial<T>::value && std::is_standard_layout<T>::value,
        "The specialization type of ConstantBuffer must be POD!");

    void SetupConstantBuffer();

    T& AcquireConstantBuffer();
    void UpdateConstantBuffer(const T& value);

protected:
    void* RawPtr() override;

private:
    std::unique_ptr<T> constantBufferValue;
};

template <typename T>
class StructuredBuffer final : public BaseStructuredBuffer {
};

template <typename T>
class ReadWriteBuffer final : public BaseReadWriteBuffer {
};

template <typename T>
class IndexBuffer final : public BaseIndexBuffer {
public:
    static_assert(std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value,
        "The specialization type of IndexBuffer must be uint16_t or uint32_t!");

    void SetupIndexBuffer(unsigned int indicesCount);

    std::vector<T>& AcquireIndexBuffer();
    void UpdateIndexBuffer(const std::vector<T>& value, unsigned int offset);

protected:
    void* RawPtr() override;

private:
    std::vector<T> indexBufferValue;
};

template <typename T>
class VertexBuffer final : public BaseVertexBuffer {
public:
    static_assert(std::is_trivial<T>::value && std::is_standard_layout<T>::value,
        "The specialization type of VertexBuffer must be POD!");

    void SetupVertexBuffer(unsigned int verticesCount);

    std::vector<T>& AcquireVertexBuffer();
    void UpdateVertexBuffer(const std::vector<T>& value, unsigned int offset);

protected:
    void* RawPtr() override;

private:
    std::vector<T> vertexBufferValue;
};

using IndexBuffer16 = IndexBuffer<uint16_t>;
using IndexBuffer32 = IndexBuffer<uint32_t>;
using MeshBuffer = VertexBuffer<math::XMFLOAT3>;

}

#include "Resources.inl"
