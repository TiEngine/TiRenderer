#pragma once

#include "BasicTypes.h"

namespace ti::backend {

class Swapchain {
public:
    struct Description {
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int refreshRate = 60;
        unsigned int bufferCount = 3;
        void* window = nullptr;
        bool fullScreen = false;
        Format colorFormat = Format::R8G8B8A8_UNORM;
        Format depthStencilFormat = Format::D24_UNORM_S8_UINT;
    };

    virtual void Resize(unsigned int width, unsigned int height) = 0;

    virtual void Present() = 0;

protected:
    Swapchain() = default;
    virtual ~Swapchain() = default;
};

}
