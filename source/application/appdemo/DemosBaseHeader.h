#pragma once

#include <Windows.h>
#include "math/Math.hpp"
#include "application/Application.h"
#include "backend/BackendContext.h"
#include "backend/Device.h"

class DemoBase {
public:
    virtual void Begin() = 0;
    virtual void Finish() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    virtual void Resize(HWND window, unsigned int width, unsigned int height) = 0;

    DemoBase() = default;
    virtual ~DemoBase() = default;

    inline ti::application::Application& GetApp()
    {
        return application;
    }

private:
    ti::application::Application application;
};

class Demo_01_Backend : public DemoBase {
public:
    void Begin() override;
    void Finish() override;
    void Update() override;
    void Draw() override;

    void Resize(HWND window, unsigned int width, unsigned int height) override;

private:
    void AutomateRotate();

    unsigned int frame = 0;
    float aspectRatio = 0.0f;

    struct VertexData {
        ti::math::XMFLOAT3 position;
        ti::math::XMFLOAT4 color;
    };
    static const std::vector<VertexData> vertices;
    static const std::vector<uint16_t> indices;

    static const std::string vertexShaderString;
    static const std::string fragmentShaderString;

    struct ObjectMVP {
        ti::math::XMFLOAT4X4 mvp =
            ti::math::XMFLOAT4X4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
    } objectMVP;
    bool updateObjectMVP = false;

    float theta = 1.5f * ti::math::XM_PI;
    float phi = ti::math::XM_PIDIV4;
    float radius = 5.0f;
    float delta = 0.01f;

    ti::backend::Viewport viewport;
    ti::backend::Scissor scissor;

    ti::backend::BackendContext* backend = nullptr;
    ti::backend::Device* device = nullptr;

    ti::backend::CommandRecorder* commandRecorder = nullptr;

    ti::backend::Shader* vertexShader = nullptr;
    ti::backend::Shader* pixelShader = nullptr;

    ti::backend::InputVertexAttributes* inputVertexAttributes = nullptr;
    ti::backend::InputIndexAttribute* inputIndexAttribute = nullptr;

    ti::backend::InputVertex* inputVertex = nullptr;
    ti::backend::InputIndex* inputIndex= nullptr;

    ti::backend::ResourceBuffer* cbObjectMVP = nullptr;

    ti::backend::DescriptorHeap* descriptorHeap = nullptr;
    ti::backend::Descriptor* descriptorForObjectMVP = nullptr;

    ti::backend::DescriptorGroup* descriptorGroup = nullptr;
    ti::backend::PipelineLayout* pipelineLayout = nullptr;

    ti::backend::PipelineState* pipelineState = nullptr;

    ti::backend::Swapchain* swapchain = nullptr;
    // Default swapchain format, see Swapchain.h for detail.
    const ti::backend::BasicFormat ColorAttachmentFormat
        = ti::backend::BasicFormat::R8G8B8A8_UNORM;
    const ti::backend::BasicFormat DepthStencilAttachmentFormat
        = ti::backend::BasicFormat::D24_UNORM_S8_UINT;

    ti::backend::ResourceImage* halfColorOutput = nullptr;
    ti::backend::DescriptorHeap* descriptorHeapRT = nullptr; // ColorOutput descriptor heap
    ti::backend::Descriptor* descriptorForHalfColorOutput = nullptr;
};
