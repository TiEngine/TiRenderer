#include "Backend.h"
#include "Configuration.h"

namespace ti::passflow {

Backend::Backend()
{
    backend = Configuration::GetReference().GetBackendType();
    context = backend::BackendContext::CreateBackend(backend);
    if (context) {
        device = context->CreateDevice({ "" /* default adaptor */ });
    }
}

Backend::~Backend()
{
    if (device && context) {
        context->DestroyDevice(device);
        backend::BackendContext::DestroyBackend(backend);
    }
}

backend::Device* Backend::Device()
{
    return device;
}

}
