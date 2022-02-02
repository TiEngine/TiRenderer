#pragma once

#include "BackendApi.h"
#include "common/DllWrapper.hpp"

namespace ti::backend {

class BackendApi BackendContext {
public:
    enum class Backend {
        DX12,
        Vulkan,
        SoftRaster
    };

    static BackendContext* CreateBackend(Backend type);
    static void DestroyBackend(Backend type);

protected:
    BackendContext() = default;
    virtual ~BackendContext() = default;
};

}
