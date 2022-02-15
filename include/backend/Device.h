#pragma once

#include "Swapchain.h"
#include "CommandAllocator.h"

namespace ti::backend {

class Device {
public:
    struct Description {
    };

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
