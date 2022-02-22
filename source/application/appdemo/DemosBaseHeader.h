#pragma once

#include <Windows.h>
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
    ti::backend::BackendContext* backend = nullptr;
    ti::backend::Device* device = nullptr;

    ti::backend::CommandRecorder* commandRecorder = nullptr;

    ti::backend::Shader* vertexShader = nullptr;
    ti::backend::Shader* pixelShader = nullptr;

    ti::backend::InputVertexAttributes* inputVertexAttributes = nullptr;

    ti::backend::Swapchain* swapchain = nullptr;
};
