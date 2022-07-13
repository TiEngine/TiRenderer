#pragma once

#include <memory>
#include <unordered_map>
#include "backend/BackendContext.h"

namespace ti::passflow {

class BaseConstantBuffer {
};

template <typename T>
class ConstantBuffer final : public BaseConstantBuffer {
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

class MeshBuffer final {
};

class IndexBuffer final {
};

struct DrawMesh final {
    std::shared_ptr<MeshBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

    std::shared_ptr<StructuredBuffer<float[3]>> normalBuffer;
    std::shared_ptr<StructuredBuffer<float[2]>> coord0Buffer;
    std::shared_ptr<StructuredBuffer<float[2]>> coord1Buffer;
};

struct DrawMaterial final {
    struct MaterialProperties final {
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

struct DrawItem final {
    std::shared_ptr<DrawMesh> drawMesh;
    std::shared_ptr<DrawMaterial> drawMaterial;
};

struct DispatchItem final {
    unsigned int threads[3] = { 1u, 1u, 1u };
};

struct Resources final {
    struct OneFrame final {
        // Buffers
        std::unordered_map<std::string, std::shared_ptr<BaseConstantBuffer>> constantBuffers;
        std::unordered_map<std::string, std::shared_ptr<BaseStructuredBuffer>> structuredBuffers;
        std::unordered_map<std::string, std::shared_ptr<BaseReadWriteBuffer>> readWriteBuffers;
        // Textures
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> texture2Ds;
        std::unordered_map<std::string, std::shared_ptr<ReadWriteTexture2D>> readWriteTexture2Ds;
        // Outputs
        std::unordered_map<std::string, std::shared_ptr<ColorOutput>> colorOutputs;
        std::unordered_map<std::string, std::shared_ptr<DepthStencilOutput>> depthStencilOutputs;
        std::unordered_map<std::string, std::shared_ptr<DisplayPresentOutput>> swapchainOutputs;
    } oneFrameResources;

    std::vector<std::shared_ptr<DrawItem>> drawItems;
    std::vector<std::shared_ptr<OneFrame>> drawItemsExtraResources;

    std::vector<std::shared_ptr<DispatchItem>> dispatchItems;
    std::vector<std::shared_ptr<OneFrame>> dispatchItemsExtraResources;
};

}
