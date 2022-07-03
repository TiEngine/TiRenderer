#pragma once

#include <memory>
#include <unordered_map>
#include "backend/BackendContext.h"

namespace ti::passflow {

class BaseConstantBuffer {
};

template <typename T>
class ConstantBuffer : public BaseConstantBuffer {
};

class BaseStructuredBuffer {
};

template <typename T>
class StructuredBuffer : public BaseStructuredBuffer {
};

class BaseReadWriteBuffer {
};

template <typename T>
class ReadWriteBuffer : public BaseReadWriteBuffer {
};

class BaseTexture {
};

class Texture2D : public BaseTexture {
};

class ReadWriteTexture2D : public BaseTexture {
};

class ColorOutput : public BaseTexture {
};

class DepthStencilOutput : public BaseTexture {
};

class DisplayPresentOutput : public BaseTexture {
};

class MeshBuffer {
};

class IndexBuffer {
};

struct DrawMesh {
    std::shared_ptr<MeshBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

    std::shared_ptr<StructuredBuffer<float[3]>> normalBuffer;
    std::shared_ptr<StructuredBuffer<float[2]>> coord0Buffer;
    std::shared_ptr<StructuredBuffer<float[2]>> coord1Buffer;
};

struct DrawMaterial {
    struct MaterialProperties {
        float albedoFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float metallicFactor = 0.5f;
        float roughnessFactor = 0.5f;
        float normalScale = 1.0f;
        float occlusionStrength = 0.0f;
        float emissiveFactor[3] = { 0.0f, 0.0f, 0.0f };
    };
    std::shared_ptr<ConstantBuffer<MaterialProperties>> materialProperties;

    std::shared_ptr<Texture2D> albedoTexture;
    std::shared_ptr<Texture2D> metallicRoughnessTexture;
    std::shared_ptr<Texture2D> normalTexture;
    std::shared_ptr<Texture2D> occlusionTexture;
    std::shared_ptr<Texture2D> emissiveTexture;
};

struct DrawItem {
    std::shared_ptr<DrawMesh> drawMesh;
    std::shared_ptr<DrawMaterial> drawMaterial;
};

struct DispatchItem {
    unsigned int threads[3] = { 1u, 1u, 1u };
    // TODO
};

struct FrameResource {
    std::vector<std::shared_ptr<DrawItem>> drawItems;         // per draw call resource
    std::vector<std::shared_ptr<DispatchItem>> dispatchItems; // per dispatch call resource

    struct PerFrame {
        std::unordered_map<std::string, std::shared_ptr<BaseConstantBuffer>> constantBuffers;
        std::unordered_map<std::string, std::shared_ptr<BaseStructuredBuffer>> structuredBuffers;
        std::unordered_map<std::string, std::shared_ptr<BaseReadWriteBuffer>> readWriteBuffers;

        std::unordered_map<std::string, std::shared_ptr<Texture2D>> texture2Ds;
        std::unordered_map<std::string, std::shared_ptr<ReadWriteTexture2D>> readWriteTexture2Ds;

        std::unordered_map<std::string, std::shared_ptr<ColorOutput>> colorOutputs;
        std::unordered_map<std::string, std::shared_ptr<DepthStencilOutput>> depthStencilOutputs;
        std::unordered_map<std::string, std::shared_ptr<DisplayPresentOutput>> presentOutputs;
    } perFrameResource; // per pass frame resource
};

}
