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

    void Resize(HWND window, unsigned int width, unsigned int height) override;

private:
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

    ti::backend::BackendContext* backend = nullptr;
    ti::backend::Device* device = nullptr;

    ti::backend::CommandRecorder* commandRecorder = nullptr;

    ti::backend::Shader* vertexShader = nullptr;
    ti::backend::Shader* pixelShader = nullptr;

    ti::backend::InputVertexAttributes* inputVertexAttributes = nullptr;

    ti::backend::InputVertex* vertexInput = nullptr;
    ti::backend::InputIndex* indexInput = nullptr;

    ti::backend::ResourceBuffer* cbObjectMVP = nullptr;

    ti::backend::Swapchain* swapchain = nullptr;
};
