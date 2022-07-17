#pragma once

#include <memory>
#include <unordered_map>
#include "backend/BackendContext.h"
//#include "common/Logger.hpp"

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
    backend::ResourceBuffer* Buffer();

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
    T constantBufferValue;
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

};

class MeshBuffer final : public DeviceHolder {
public:
    MeshBuffer() = default;
    ~MeshBuffer() override;

    void UploadMeshBuffer();

    backend::InputVertex* Buffer();

protected:
    void SetupGPU();
    virtual void* RawPtr();

private:
    backend::InputVertex::Description description{ 0u, 0u };
    backend::InputVertex* vertex = nullptr;
};

}

#include "Resources.inl"
