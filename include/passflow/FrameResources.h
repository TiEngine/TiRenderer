#pragma once

#include "Resources.h"

namespace ti::passflow {

struct DrawMesh final {
    std::shared_ptr<IndexBuffer32> indexBuffer;
    std::shared_ptr<MeshBuffer> positionBuffer;

    std::shared_ptr<StructuredBuffer<float[3]>> normalBuffer;
    std::shared_ptr<StructuredBuffer<float[2]>> coord0Buffer;
    std::shared_ptr<StructuredBuffer<float[2]>> coord1Buffer;

    std::shared_ptr<ReadWriteBuffer<float[3]>> positionResolvedBuffer;
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
    // The total number of threads in a group needs to be a multiple of 64.
    unsigned int threadsEachGroup[3] = { 64u, 1u, 1u };
    unsigned int threadGroups[3] = { 1u, 1u, 1u };
};

struct FrameResources final {
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
