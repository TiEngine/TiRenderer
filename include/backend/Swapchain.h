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
        Format format = Format::R8G8B8A8_UNORM;
        void* window = nullptr;
        bool fullScreen = false;
    };
    virtual void Setup(Description description) = 0;

protected:
    Swapchain() = default;
    virtual ~Swapchain() = default;
};

}
