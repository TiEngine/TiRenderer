#include "Configuration.h"

namespace ti::passflow {

Configuration::Configuration()
{
    // Default initialize...
    #if WIN32
    backendType = backend::BackendContext::Backend::DX12;
    #elif __linux__
    backendType = backend::BackendContext::Backend::Vulkan;
    #else
    backendType = backend::BackendContext::Backend::SoftRaster;
    #endif
}

Configuration::~Configuration()
{
}

void Configuration::SetBackendType(backend::BackendContext::Backend type)
{
    backendType = type;
}

backend::BackendContext::Backend Configuration::GetBackendType() const
{
    return backendType;
}

}
