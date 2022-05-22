#pragma once

#include "BasicTypes.h"

namespace ti::backend {

class Swapchain {
public:
    struct Description {
        void* window;
        unsigned int width;
        unsigned int height;
        unsigned int bufferCount;
        unsigned int refreshRate;
        BasicFormat colorFormat;
        BasicFormat depthStencilFormat;
        ClearValue colorClearValue;
        ClearValue depthStencilClearValue;
        bool isEnabledDepthStencil;
        bool fullScreen;

        Description(
            void* window,
            unsigned int width,
            unsigned int height,
            unsigned int bufferCount = 3,
            unsigned int refreshRate = 60,
            bool isEnabledDepthStencil = true,
            BasicFormat colorFormat = BasicFormat::R8G8B8A8_UNORM,
            BasicFormat depthStencilFormat = BasicFormat::D24_UNORM_S8_UINT,
            ClearValue colorClearValue = {},
            ClearValue depthStencilClearValue = {},
            bool fullScreen = false)
            : window(window)
            , width(width)
            , height(height)
            , bufferCount(bufferCount)
            , refreshRate(refreshRate)
            , colorFormat(colorFormat)
            , colorClearValue(colorClearValue)
            , depthStencilFormat(depthStencilFormat)
            , depthStencilClearValue(depthStencilClearValue)
            , isEnabledDepthStencil(isEnabledDepthStencil)
            , fullScreen(fullScreen)
        {}
    };

    virtual void Resize(unsigned int width, unsigned int height) = 0;
    virtual void Present() = 0;

protected:
    Swapchain() = default;
    virtual ~Swapchain() = default;
};

}
