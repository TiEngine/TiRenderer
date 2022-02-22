#pragma once

#include "BasicTypes.h"

namespace ti::backend {

class Swapchain {
public:
    struct Description {
        void* window;
        unsigned int width;
        unsigned int height;
        unsigned int refreshRate = 60;
        unsigned int bufferCount = 3;
        bool fullScreen = false;
        Format colorFormat = Format::R8G8B8A8_UNORM;
        Format depthStencilFormat = Format::D24_UNORM_S8_UINT;

        Description(void* window, unsigned int width, unsigned int height,
            unsigned int refreshRate = 60, unsigned int bufferCount = 3,
            bool fullScreen = false,
            Format colorFormat = Format::R8G8B8A8_UNORM,
            Format depthStencilFormat = Format::D24_UNORM_S8_UINT)
            : window(window), width(width), height(height)
            , refreshRate(refreshRate), bufferCount(bufferCount)
            , fullScreen(fullScreen)
            , colorFormat(colorFormat)
            , depthStencilFormat(depthStencilFormat) {}
    };

    virtual void Resize(unsigned int width, unsigned int height) = 0;

    virtual void Present() = 0;

protected:
    Swapchain() = default;
    virtual ~Swapchain() = default;
};

}
