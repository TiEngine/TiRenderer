#pragma once

#include "BackendApi.h"

namespace ti::backend {
class Device;

class BackendContext {
public:
    enum class Backend {
        DX12,
        Vulkan,
        SoftRaster
    };

    BackendApi static BackendContext* CreateBackend(Backend type);
    BackendApi static void DestroyBackend(Backend type);

    virtual Device* CreateDevice() = 0;
    virtual bool DestroyDevice(Device* device) = 0;

protected:
    BackendContext() = default;
    virtual ~BackendContext() = default;
};
}
