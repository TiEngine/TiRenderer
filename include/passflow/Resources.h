#pragma once

#include <memory>
#include <unordered_map>
#include "math/Math.hpp"
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

template <typename T>
class ConstantBuffer final : public BaseConstantBuffer {
public:
    void SetupConstantBuffer();

    T& AcquireConstantBuffer();
    void UpdateConstantBuffer(const T& value);

protected:
    void* RawPtr() override;

private:
    std::unique_ptr<T> constantBufferValue;
};

class BaseStructuredBuffer {
};

template <typename T>
class StructuredBuffer final : public BaseStructuredBuffer {
};

class BaseReadWriteBuffer {
};

template <typename T>
class ReadWriteBuffer final : public BaseReadWriteBuffer {
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

class BaseIndexBuffer : public DeviceHolder {
};

class IndexBuffer final : public BaseIndexBuffer {
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

class MeshBuffer final : public BaseVertexBuffer {
public:
    void SetupMeshBuffer(unsigned int verticesCount);

    std::vector<math::XMFLOAT3>& AcquireMeshBuffer();
    void UpdateMeshBuffer(const std::vector<math::XMFLOAT3>& value, unsigned int offset);

protected:
    void* RawPtr() override;

private:
    std::vector<math::XMFLOAT3> meshPositionBuffer;
};

}

#include "Resources.inl"
