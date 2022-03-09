#pragma once

#include "BasicTypes.h"

namespace ti::backend {

class Swapchain {
public:
    struct Description {
        void* window;
        unsigned int width;
        unsigned int height;
        unsigned int refreshRate;
        unsigned int bufferCount;
        BasicFormat colorFormat;
        bool fullScreen;

        Description(
            void* window,
            unsigned int width,
            unsigned int height,
            unsigned int refreshRate = 60,
            unsigned int bufferCount = 3,
            BasicFormat colorFormat = BasicFormat::R8G8B8A8_UNORM,
            bool fullScreen = false)
            : window(window)
            , width(width)
            , height(height)
            , refreshRate(refreshRate)
            , bufferCount(bufferCount)
            , colorFormat(colorFormat)
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
