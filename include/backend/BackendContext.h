#pragma once

#include "BackendApi.h"
#include "common/DllWrapper.hpp"

namespace ti::backend {

class Device;

class BackendApi BackendContext {
public:
    enum class Backend {
        DX12,
        Vulkan,
        SoftRaster
    };

    static BackendContext* CreateBackend(Backend type);
    static void DestroyBackend(Backend type);

    virtual Device* CreateDevice() = 0;
    virtual bool DestroyDevice(Device* device) = 0;

protected:
    BackendContext() = default;
    virtual ~BackendContext() = default;
};

}
