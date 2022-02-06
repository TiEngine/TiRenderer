#pragma once

#include "Swapchain.h"

namespace ti::backend {

class Device {
public:
    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* swapchain) = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};

}
