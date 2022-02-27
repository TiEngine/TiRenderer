#pragma once

#include "Shader.h"
#include "Swapchain.h"
#include "CommandRecorder.h"
#include "InputVertexAttributes.h"

namespace ti::backend {
class Device {
public:
    struct Description {
        std::string device; // Selected adapter device name.
    };

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* shader) = 0;

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* swapchain) = 0;

    virtual CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) = 0;
    virtual bool DestroyCommandRecorder(CommandRecorder* commandRecorder) = 0;

    virtual InputVertexAttributes* CreateInputVertexAttributes(InputVertexAttributes::Description description) = 0;
    virtual bool DestroyInputVertexAttributes(InputVertexAttributes* inputVertexAttributes) = 0;

    virtual void WaitIdle() = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};
}
