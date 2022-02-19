#pragma once

#include "Shader.h"
#include "Swapchain.h"
#include "CommandAllocator.h"

namespace ti::backend {

class Device {
public:
    struct Description {
    };

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* shader) = 0;

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* swapchain) = 0;

    virtual CommandAllocator* CreateCommandAllocator(CommandAllocator::Description description) = 0;
    virtual bool DestroyCommandAllocator(CommandAllocator* commandAllocator) = 0;

    virtual void WaitIdle() = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};

}
