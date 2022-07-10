#include "Configuration.h"

namespace ti::passflow {

Configuration::Configuration()
{
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

bool Configuration::SetBackendType(backend::BackendContext::Backend type)
{
    if (backendTypeGot) {
        return false;
    }
    backendType = type;
    return true;
}

backend::BackendContext::Backend Configuration::GetBackendType() const
{
    backendTypeGot = true;
    return backendType;
}

bool Configuration::SetMultipleBufferingCount(unsigned int count)
{
    if (multipleBufferingCountGot) {
        return false;
    }
    multipleBufferingCount = count;
    return true;
}

unsigned int Configuration::GetMultipleBufferingCount() const
{
    multipleBufferingCountGot = true;
    return multipleBufferingCount;
}

}
